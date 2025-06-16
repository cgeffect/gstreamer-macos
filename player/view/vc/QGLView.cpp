#include "QGLView.h"
#include <QOpenGLContext>
#include <QSurfaceFormat>
#include <iostream>
#include "view/vc/GLInfo.h"
namespace vleap {
const GLfloat vertices[] = {
    -1.0f, -1.0f, 0.0f, // 左下角
    1.0f, -1.0f, 0.0f,  // 右下角
    -1.0f, 1.0f, 0.0f,  // 左上角
    1.0f, 1.0f, 0.0f    // 右上角
};

// const GLfloat texCoords[] = {
//     0.0f, 1.0f, // 左下角
//     1.0f, 1.0f, // 右下角
//     0.0f, 0.0f, // 左上角
//     1.0f, 0.0f  // 右上角
// };

const std::string vert2D = R"(
    #version 110

    attribute vec3 aPosition;
    uniform mat4 uModelViewProjectionMatrix;
    varying vec2 TexCoord;
    void main() {
        gl_Position = uModelViewProjectionMatrix * vec4(aPosition, 1.0);

        vec2 tempTexCoord = aPosition.xy * vec2(0.5) + vec2(0.5);
        TexCoord = vec2(tempTexCoord.x, 1.0 - tempTexCoord.y);
    }
)";

const std::string frag2D = R"(
    #version 110

    uniform sampler2D uTexture;
    varying vec2 TexCoord;
    void main() {
        vec4 color = texture2D(uTexture, TexCoord); // bgra
        gl_FragColor = vec4(color.b, color.g, color.r, color.a);
    }
)";

// 有参数的构造函数
QGLView::QGLView(QWidget *parent) :
    QOpenGLWidget(parent) {
    shaderProgram = nullptr;
    // 设置OpenGL版本为3.3
    QSurfaceFormat format;
    format.setMajorVersion(3);
    format.setMinorVersion(0);
    format.setRenderableType(QSurfaceFormat::RenderableType::OpenGL);
    // 设置为核心模式
    format.setProfile(QSurfaceFormat::CoreProfile);
    // 将设置好的格式应用到QOpenGLWidget
    setFormat(format);
}

void QGLView::initializeGL() {
    // QOpenGLFunctions *f = QOpenGLContext::currentContext()->functions();
    // f->initializeOpenGLFunctions(); // 初始化 OpenGL 函数
    initializeOpenGLFunctions();
    GLInfo::printOpenGLVersion();

    // 创建和编译 Shader 程序
    shaderProgram = new QOpenGLShaderProgram(this);
    bool vertexStatus = shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vert2D.c_str());
    if (!vertexStatus) {
        qDebug() << "Vertex shader compilation failed:" << shaderProgram->log();
    }
    bool fragStatus = shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, frag2D.c_str());
    if (!fragStatus) {
        qDebug() << "Fragment shader compilation failed:" << shaderProgram->log();
    }
    if (!shaderProgram->link()) {
        qDebug() << "Shader program linking failed:" << shaderProgram->log();
    }
    int aPositionIndex = shaderProgram->attributeLocation("aPosition");

    // 创建和绑定 VAO
    glGenVertexArrays(1, &VAO); // VAO 在3.3版本核心模式必须使用VAO
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(aPositionIndex);
    glVertexAttribPointer(aPositionIndex, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindVertexArray(0);

    // 创建纹理对象
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void QGLView::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shaderProgram->bind();
    // 激活纹理单元 0
    glActiveTexture(GL_TEXTURE0);
    // 绑定纹理对象到激活的纹理单元
    glBindTexture(GL_TEXTURE_2D, textureId);
    // 设置着色器中的采样器统一变量，使其指向纹理单元 0
    shaderProgram->setUniformValue("uTexture", 0);
    shaderProgram->setUniformValue("uModelViewProjectionMatrix", zoomOut(width(), height(), texWidth, texHeight));

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
    shaderProgram->release();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void QGLView::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
    printf("resizeGL: %d, %d\n", w, h);

    shaderProgram->bind();
    shaderProgram->setUniformValue("uModelViewProjectionMatrix", zoomOut(w, h, texWidth, texHeight));
    shaderProgram->release();
}

// 放大
QMatrix4x4 QGLView::zoomIn(int surfaceWidth, int surfaceHeight, int imageWidth, int imageHeight) {
    // 图片的宽高比
    float imageAspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    // 窗口的宽高比
    float surfaceAspectRatio = static_cast<float>(surfaceWidth) / static_cast<float>(surfaceHeight);
    printf("imageAspectRatio: %f, surfaceAspectRatio: %f\n", imageAspectRatio, surfaceAspectRatio);
    QMatrix4x4 projectionMatrix;
    if (imageAspectRatio > surfaceAspectRatio) {
        // 图片宽高比大于屏幕，按照宽度填满计算高度
        float tb = imageAspectRatio / surfaceAspectRatio;
        projectionMatrix.ortho(-1.0f, 1.0f, -tb, tb, -1.0f, 1.0f);

        int scaledWidth, scaledHeight;
        getZoomOutMatrix(surfaceWidth, surfaceHeight, scaledWidth, scaledHeight);
        float scale = height() / scaledHeight;
        projectionMatrix.scale(scale, scale);
    } else {
        // 图片宽高比小于等于屏幕，按照高度填满计算宽度
        float tb = surfaceAspectRatio / imageAspectRatio;
        projectionMatrix.ortho(-tb, tb, -1.0f, 1.0f, -1.0f, 1.0f);

        int scaledWidth, scaledHeight;
        getZoomOutMatrix(surfaceWidth, surfaceHeight, scaledWidth, scaledHeight);
        // 计算从等比缩小状态到填满画布的放大比例
        float scale = width() / scaledWidth; // 根据窗口高度调整图片高度
        projectionMatrix.scale(scale, scale);
    }

    return projectionMatrix;
}

// 缩小
QMatrix4x4 QGLView::zoomOut(int surfaceWidth, int surfaceHeight, int imageWidth, int imageHeight) {
    // 图片的宽高比
    float imageAspectRatio = static_cast<float>(imageWidth) / static_cast<float>(imageHeight);
    // 窗口的宽高比
    float surfaceAspectRatio = static_cast<float>(surfaceWidth) / static_cast<float>(surfaceHeight);

    QMatrix4x4 projectionMatrix;
    if (imageAspectRatio > surfaceAspectRatio) {
        // 图片宽高比大于屏幕,按照宽度填满计算高度
        float tb = imageAspectRatio / surfaceAspectRatio;
        projectionMatrix.ortho(-1.0f, 1.0f, -tb, tb, -1.0f, 1.0f);
    } else {
        // 图片宽高比小于等于屏幕,按照高度填满计算宽度
        float tb = surfaceAspectRatio / imageAspectRatio;
        projectionMatrix.ortho(-tb, tb, -1.0f, 1.0f, -1.0f, 1.0f);
    }

    // 视图矩阵
    // QMatrix4x4 viewMatrix;
    // viewMatrix.lookAt(QVector3D(0.0f, 0.0f, -1.0f), QVector3D(0.0f, 0.0f, 0.0f), QVector3D(0.0f, 1.0f, 0.0f));

    // 模型矩阵
    // QMatrix4x4 modelMatrix;

    return projectionMatrix;
}

void QGLView::onUpdateTexture(GLubyte *newRgbaData, int width, int height) {
    QMutexLocker locker(&mutex);
    texWidth = width;
    texHeight = height;
    glBindTexture(GL_TEXTURE_2D, textureId);
    if (dataUpdated) {
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, newRgbaData);
    } else {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, newRgbaData);
        dataUpdated = true;
    }
    glBindTexture(GL_TEXTURE_2D, 0);

    condition.wakeOne();

    update(); // 触发paintGL
}

QGLView::~QGLView() {
    makeCurrent();

    if (shaderProgram) {
        delete shaderProgram;
    }
    glDeleteTextures(1, &textureId);
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    doneCurrent();
}

} // namespace vleap

// int aPositionIndex = m_program->attributeLocation("aPosition");
// int aTexCoordIndex = m_program->attributeLocation("aTexCoord");

//    if (m_texture == nullptr) {
//        m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
//        m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);         // 设置纹理格式为 RGBA
//        m_texture->setSize(texWidth, texHeight);                   // 设置纹理尺寸
//        m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);       // 设置纹理环绕模式
//        m_texture->setMinificationFilter(QOpenGLTexture::Linear);  // 设置纹理缩小过滤器
//        m_texture->setMagnificationFilter(QOpenGLTexture::Linear); // 设置纹理放大过滤器
//        m_texture->allocateStorage();                              // 分配存储空间
//    }
//    m_texture->setData(0, 0, QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, rgbaData);
