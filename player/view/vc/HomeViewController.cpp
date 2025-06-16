#include "HomeViewController.h"
#include <QVBoxLayout>
#include <QFileDialog>
#include <QStandardPaths>
namespace vleap {
const static float SLIDER_MAX = 10000.0f;
static std::string formatMilliseconds(int milliseconds) {
    // 计算分钟数
    int minutes = milliseconds / (1000 * 60);
    // 计算除去分钟后剩余的毫秒数
    milliseconds %= (1000 * 60);
    // 计算秒数
    int seconds = milliseconds / 1000;
    // 计算除去秒后剩余的毫秒数
    milliseconds %= 1000;

    // 使用 std::ostringstream 来格式化输出
    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << minutes << ":"
        << std::setfill('0') << std::setw(2) << seconds << ":"
        << std::setfill('0') << std::setw(3) << milliseconds;

    return oss.str();
}
HomeViewController::HomeViewController(QWidget *parent) :
    QWidget(parent) {

    setupUI();
}

void HomeViewController::init(std::string url) {
    // 启动OpenGL渲染线程
    playerController = new PlayerController(glView);
    connect(playerController, &PlayerController::updateTimestamp,
            this, &HomeViewController::onUpdateTimestamp,
            Qt::QueuedConnection);
    connect(
        playerController, &PlayerController::playFinished, // 发送者(信号)
        this, &HomeViewController::onPlayFinished,         // 接收者(槽函数)
        Qt::QueuedConnection                               // 接收者的线程执行
    );

    playerController->init(url);
}

void HomeViewController::setupUI() {
    // 创建布局
    rootLayout = new QVBoxLayout();

    // 顶部工具栏
    QHBoxLayout *toolLayout = new QHBoxLayout();
    rootLayout->addLayout(toolLayout);

    {
        // 左边区域
        QHBoxLayout *leftHorizontalLayout = new QHBoxLayout();
        QWidget *leftWidget = new QWidget();
        leftWidget->setFixedWidth(200);
        leftHorizontalLayout->addWidget(leftWidget);

        auto fileBtn = new QPushButton("编辑协议", leftWidget);
        fileBtn->resize(80, 30);
        connect(fileBtn, &QPushButton::clicked, [=]() {
            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString filePath = QFileDialog::getOpenFileName(this, "选择文件", desktopPath, "JSON 文件 (*.json)");
            if (!filePath.isEmpty()) {
                qDebug() << "选择的文件路径: " << filePath;
            }
        });

        auto initBtn = new QPushButton("更换协议", leftWidget);
        initBtn->resize(80, 30);
        initBtn->move(100, 0);
        connect(initBtn, &QPushButton::clicked, [=]() {
            QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
            QString filePath = QFileDialog::getOpenFileName(this, "选择文件", desktopPath, "JSON 文件 (*.json)");
            if (!filePath.isEmpty()) {
                qDebug() << "选择的文件路径: " << filePath;
            }
        });
        toolLayout->addLayout(leftHorizontalLayout);
    }
    {
        // 中间区域
        glView = new QGLView(this);
        glView->setFixedSize(600, 600); // 设置固定大小
        toolLayout->addWidget(glView);
    }
    {
        // 右边区域
        QHBoxLayout *rightHorizontalLayout = new QHBoxLayout();
        QWidget *rightWidget = new QWidget();
        rightWidget->setFixedWidth(200);
        rightHorizontalLayout->addWidget(rightWidget);
        auto rightBtn = new QPushButton("属性", rightWidget);
        rightBtn->resize(50, 30);
        toolLayout->addLayout(rightHorizontalLayout);
    }

    // 底部工具栏
    auto hLayout = new QHBoxLayout();
    button = new QPushButton("播放", this);
    button->setCheckable(true);
    button->setChecked(false);
    connect(button, &QPushButton::toggled, [this](bool checked) {
        if (checked) {
            playerController->play();
            button->setText("暂停");
            qDebug() << "Button is unchecked!";
        } else {
            qDebug() << "Button is checked!";
            playerController->pause();
            button->setText("播放");
        }
    });

    hLayout->addWidget(button);

    {
        label = new QLabel("00:00:000", this);
        label->setFixedWidth(70);
        hLayout->addWidget(label);

        // 创建滑动条
        qSlider = new QSlider(Qt::Horizontal, this); // 水平滑动条
        qSlider->setRange(0, SLIDER_MAX);            // 设置滑动条的范围
        qSlider->setValue(0);                        // 设置默认值
        hLayout->addWidget(qSlider);
        // 连接滑动条的信号到槽函数
        connect(qSlider, &QSlider::valueChanged, this, &HomeViewController::onSliderValueChanged);
        connect(qSlider, &QSlider::sliderPressed, this, &HomeViewController::onSliderPressed);
        connect(qSlider, &QSlider::sliderReleased, this, &HomeViewController::onSliderReleased);
        connect(qSlider, &QSlider::sliderMoved, this, &HomeViewController::onSliderMoved);
    }
    rootLayout->addLayout(hLayout);
    // 设置布局
    setLayout(rootLayout);
}

void HomeViewController::onUpdateTimestamp(int64_t timestamp, int64_t durationMs) {
    durationMs_ = durationMs;
    float precent = float(timestamp) / durationMs;
    // QSlider只接受整形值, 所有小数值将被截断
    qSlider->setValue(precent * SLIDER_MAX);
}

void HomeViewController::onPlayFinished() {
    button->setChecked(false);
}

// 滑动条值变化时的槽函数
void HomeViewController::onSliderValueChanged(int value) {
    float ptsMs = value / SLIDER_MAX * durationMs_;
    std::string formatTime = formatMilliseconds(ptsMs);
    QString qFormatTime = QString::fromStdString(formatTime);
    label->setText(qFormatTime);
}

void HomeViewController::onSliderPressed() {
    isSliderPressed_ = true;
    playerController->holdSeek(true);
}

void HomeViewController::onSliderReleased() {
    float seekValue_ = qSlider->value();
    float ptsMs = seekValue_ / SLIDER_MAX * durationMs_;
    playerController->seekTo(ptsMs, true);
    playerController->holdSeek(false);
    isSliderPressed_ = false;
    button->setText("播放");
    button->setChecked(false);
}

void HomeViewController::onSliderMoved(int position) {
    int64_t ptsMs = int64_t(position / SLIDER_MAX * durationMs_);
    // playerController->seekTo(ptsMs, false);
    printf("onSliderMoved %lld\n", ptsMs);
}

} // namespace vleap
