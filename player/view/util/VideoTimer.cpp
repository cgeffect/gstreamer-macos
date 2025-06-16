#include "VideoTimer.h"

namespace vleap {
const static int kFrameDuration = 40;
VideoTimer::VideoTimer(QObject *parent) :
    QObject(parent), m_currentTimestamp(0) {
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &VideoTimer::onTimerTimeout);
}

VideoTimer::~VideoTimer() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void VideoTimer::play() {
    if (!m_timer->isActive()) {
        m_timer->start(kFrameDuration); // 单位毫秒
    }
}

void VideoTimer::pause() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
}

void VideoTimer::seek(int64_t timestamp) {
    m_currentTimestamp = timestamp;
    emit timestampUpdated(m_currentTimestamp);
}

void VideoTimer::stop() {
    if (m_timer->isActive()) {
        m_timer->stop();
    }
    m_currentTimestamp = 0;
//    emit timestampUpdated(m_currentTimestamp);
}

void VideoTimer::onTimerTimeout() {
    emit timestampUpdated(m_currentTimestamp);
    m_currentTimestamp += kFrameDuration;
}

int64_t VideoTimer::getCurrentTimestamp() const {
    return m_currentTimestamp;
}
} // namespace vleap
