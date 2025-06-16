#ifndef HomeViewController_H
#define HomeViewController_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "view/vc/PlayerController.h"
#include "view/vc/QGLView.h"

namespace vleap {

class HomeViewController : public QWidget {
    Q_OBJECT

public:
    explicit HomeViewController(QWidget *parent = nullptr);
    void init(std::string url);

private:
    QVBoxLayout *rootLayout;
    QLabel *label;
    QPushButton *button;
    QSlider *qSlider; // 添加滑动条

    QGLView *glView; // 添加 OpenGLWidget
    void setupUI();

private slots:
    void onSliderValueChanged(int value); // 滑动条值变化时的槽函数
    void onSliderPressed();
    void onSliderReleased();
    void onSliderMoved(int position);
    // void onSliderActionTriggered(int action);
    // void onSliderPageStepSubtracted();

    void onUpdateTimestamp(int64_t timestamp, int64_t durationMs);

    void onPlayFinished();

private:
    PlayerController *playerController;
    float durationMs_;
    bool isSliderPressed_;
    //    float seekValue_;
};
} // namespace vleap
#endif // HomeViewController_H
