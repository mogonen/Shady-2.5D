#ifndef FRAMEBUFFERWRAPPER_H
#define FRAMEBUFFERWRAPPER_H

#include <QGLFunctions>


class FrameBufferWrapper : public QGLFunctions
{
public:
    FrameBufferWrapper(int w, int h);
    void bind();
    void release();
    GLuint texture(){return m_texture;}
private:
    GLuint m_texture;
    GLuint m_fbo;
    GLuint m_depth_rb;
    int m_widht;
    int m_height;

};

#endif // FRAMEBUFFERWRAPPER_H
