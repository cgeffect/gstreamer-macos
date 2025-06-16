#ifndef VIDEOTIMERCONTROLLER_H
#define VIDEOTIMERCONTROLLER_H

#include <QObject>
#include <QTimer>

namespace vleap {
class VideoTimer : public QObject {
    Q_OBJECT
public:
    explicit VideoTimer(QObject *parent = nullptr);
    ~VideoTimer();

    void play();
    void pause();
    void seek(int64_t timestamp);
    void stop();
    int64_t getCurrentTimestamp() const;

signals:
    void timestampUpdated(int64_t timestamp);

private slots:
    void onTimerTimeout();

private:
    QTimer *m_timer;
    int64_t m_currentTimestamp;
};

} // namespace vleap

#endif // VIDEOTIMERCONTROLLER_H
