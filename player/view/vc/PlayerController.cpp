
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

    // 初始化解码器
    if (!decoder_.init("/Users/jason/Desktop/test10.mp4")) {
        std::cerr << "Failed to initialize decoder" << std::endl;
        return;
    }
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
        std::vector<uint8_t> frameData;
        int width = 0;
        int height = 0;
        if (!decoder_.DecodeOnFrame(frameData, width, height)) {
            std::cerr << "Failed to decode frame at timestamp: " << timestamp << std::endl;
        }
        emit updateTexture(frameData.data(), width, height);
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


/*

#include "Decoder.h"
#include <iostream>

int main() {
    Decoder decoder;
    if (decoder.init("./30_50.mp4", "./test_live.yuv")) {
        std::cout << "Decoder initialized successfully." << std::endl;
        // 解码一定数量的帧
        for (int i = 0; i < 100; ++i) {
            if (!decoder.DecodeOnFrame()) {
                std::cout << "Decoding finished or failed." << std::endl;
                break;
            }
        }
        decoder.stop();
    } else {
        std::cout << "Decoder initialization failed." << std::endl;
    }
    return 0;
}
*/