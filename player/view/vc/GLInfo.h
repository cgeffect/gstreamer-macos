
#pragma once
#include <QOpenGLFunctions>

namespace vleap {

class GLInfo {
public:
    static void checkGLError() {
        GLenum error;
        while ((error = glGetError()) != GL_NO_ERROR) {
            switch (error) {
            case GL_INVALID_ENUM:
                qDebug() << "GL_INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                qDebug() << "GL_INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                qDebug() << "GL_INVALID_OPERATION";
                break;
            case GL_OUT_OF_MEMORY:
                qDebug() << "GL_OUT_OF_MEMORY";
                break;
            case GL_STACK_UNDERFLOW:
                qDebug() << "GL_STACK_UNDERFLOW";
                break;
            case GL_STACK_OVERFLOW:
                qDebug() << "GL_STACK_OVERFLOW";
                break;
            default:
                qDebug() << "Unknown OpenGL error:" << error;
                break;
            }
        }
    }
    static void printOpenGLVersion() {
        const GLubyte *version = glGetString(GL_VERSION);
        const GLubyte *renderer = glGetString(GL_RENDERER);
        const GLubyte *vendor = glGetString(GL_VENDOR);

        std::cout << "OpenGL Version: " << reinterpret_cast<const char *>(version) << std::endl;
        std::cout << "Renderer: " << reinterpret_cast<const char *>(renderer) << std::endl;
        std::cout << "Vendor: " << reinterpret_cast<const char *>(vendor) << std::endl;

        // auto surfaceFormat = QSurfaceFormat::defaultFormat();
        // auto surfaceFormat = format();
        // auto version = surfaceFormat.version();
        // auto major = surfaceFormat.majorVersion();
        // auto minor = surfaceFormat.minorVersion();
        // auto profile = surfaceFormat.profile();
    }
};

} // namespace vleap
