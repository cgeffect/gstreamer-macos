#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_3_3_Core>

// #include <QThread>
#include <QMutex>
#include <QWaitCondition>

// #include "VideoDecoderThread.h"
namespace vleap {

// 通过继承QOpenGLFunctions/QOpenGLFunctions_3_3_Core的方式, 避免使用QOpenGLContext::currentContext()->functions()
class QGLView : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    // 有参数的构造函数
    QGLView(QWidget *parent);
    ~QGLView();

protected:
    virtual void initializeGL() override;
    virtual void resizeGL(int w, int h) override;
    virtual void paintGL() override;

private:
    QOpenGLShaderProgram *shaderProgram;
    // QOpenGLBuffer vboVertices;
    // QOpenGLBuffer vboTexCoords;
    // QOpenGLVertexArrayObject vaoBuffer;

    // QOpenGLTexture *m_texture;
    unsigned int textureId;

    GLuint VAO = 0;
    GLuint VBO = 0;
    // 放大
    QMatrix4x4 zoomIn(int surfaceWidth, int surfaceHeight, int imageWidth, int imageHeight);
    // 缩小
    QMatrix4x4 zoomOut(int surfaceWidth, int surfaceHeight, int imageWidth, int imageHeight);

    void getZoomOutMatrix(int w, int h, int &outW, int &outH) {
        float imageAspectRatio = static_cast<float>(720) / static_cast<float>(1280);
        float surfaceAspectRatio = static_cast<float>(w) / static_cast<float>(h);

        float scaledWidth, scaledHeight;

        if (imageAspectRatio > surfaceAspectRatio) {
            // 图片宽高比大于画布宽高比，宽度填满画布
            scaledWidth = w;
            scaledHeight = w / imageAspectRatio;
        } else {
            // 图片宽高比小于或等于画布宽高比，高度填满画布
            scaledWidth = h * imageAspectRatio;
            scaledHeight = h;
        }

        outW = scaledWidth;
        outH = scaledHeight;
    }

public slots:
    void onUpdateTexture(GLubyte *rgbaData, int width, int height);

private:
    // QOpenGLTexture *texture;
    // GLubyte *rgbaData;
    QMutex mutex;
    QWaitCondition condition;
    bool dataUpdated = false;
    int texWidth = 0;
    int texHeight = 0;
};
} // namespace vleap
#endif // OPENGLWIDGET_H
