
#include "PlayerController.h"
#include <fstream>
#include "view/util/FileUtil.h"

namespace vleap {
PlayerController::PlayerController(QGLView *widget) :
    openGLWidget(widget) {
}

void PlayerController::init(std::string jsonUrl) {

    // 启动解码线程
    connect(
        this, &PlayerController::updateTexture,  // 发送者(信号)
        openGLWidget, &QGLView::onUpdateTexture, // 接收者(槽函数)
        Qt::QueuedConnection                     // 接收者的线程执行
    );

    // 启动操作队列
    operationQueue_.setMaxConcurrentOperationCount(1);
    operationQueue_.startWorker();

    // 创建定时器控制器
    videoTimer_ = new VideoTimer(this);
    connect(videoTimer_, &VideoTimer::timestampUpdated, this, &PlayerController::onTimestampUpdated);

    // 渲染第一帧
    onTimestampUpdated(0);
}

void PlayerController::onTimestampUpdated(int64_t timestamp) {
    if (timestamp > durationMs_) {
        emit playFinished();
        videoTimer_->stop();
        return;
    }
    // 处理时间戳更新
//    std::cout << "Timestamp updated: " << timestamp << std::endl;
    // 添加任务到队列
    async::Operation task;
    task.setTaskBlock([this, timestamp]() {
        // int renderRet = root_->forceFlush(timestamp);
        // if (renderRet != 0) {
        //     std::cout << "task forceFlush error: " << timestamp << std::endl;
        // }
        // root_->readPixels(root_->width(), root_->height(), imageData.data());
        // emit updateTexture(imageData.data(), root_->width(), root_->height());
    });
    task.setCompletionBlock([timestamp]() {
        std::cout << "task finished: " << timestamp << std::endl;
    });
    operationQueue_.addTaskWithBlock(task);

    emit updateTimestamp(timestamp, durationMs_);
    timestampMs_ = timestamp;
}
void PlayerController::play() {
    // 启动定时器
    videoTimer_->play();
}
void PlayerController::pause() {
    // 暂停定时器
    videoTimer_->pause();
}
void PlayerController::holdSeek(bool isSeek) {
    if (isSeek) {
        pause();
    } else {
        //        videoTimer_->play();
    }
    isSeeking = isSeek;
}

void PlayerController::seekTo(int64_t ptsMs, bool isSync) {
    operationQueue_.cancelAllTask();
    videoTimer_->seek(ptsMs);
    onTimestampUpdated(ptsMs);
}

void PlayerController::destroy() {
    if (videoTimer_) {
        // 销毁播放器
        videoTimer_->stop();
        operationQueue_.cancelAllTask();
        operationQueue_.waitForDone();
        delete videoTimer_;
        videoTimer_ = nullptr;
    }
}

PlayerController::~PlayerController() {
    destroy();
}

} // namespace vleap
