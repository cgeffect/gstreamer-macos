#pragma once
#include <QObject>
#include <string>
#include <vector>

#include "view/vc/QGLView.h"
#include "view/util/thread/OperationQueue.h"
#include "view/util/thread/QTaskQueueThread.h"
#include "view/util/VideoTimer.h"
#include "decode/Decoder.h"

namespace vleap {
class PlayerController : public QObject {
    Q_OBJECT
public:
    PlayerController(QGLView *widget);
    ~PlayerController();

    void init(std::string jsonUrl);
    void play();
    void pause();
    void holdSeek(bool isSeek);
    void seekTo(int64_t ptsMs, bool isSync);
    void destroy();
    int64_t getTimestamp() {
        return timestampMs_;
    }

signals:
    void updateTexture(GLubyte *rgbaData, int width, int height);
    void updateTimestamp(int64_t timestamp, int64_t durationMs);
    void playFinished();
private slots:
    void onTimestampUpdated(int64_t timestamp);

private:
    async::OperationQueue operationQueue_;
    QGLView *openGLWidget;

    std::vector<unsigned char> imageData;
    VideoTimer *videoTimer_;
    int64_t durationMs_ = 0;
    int64_t timestampMs_ = 0;
    bool isSeeking = false;

    Decoder decoder_;
};

} // namespace vleap
