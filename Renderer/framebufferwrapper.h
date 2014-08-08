#ifndef FRAMEBUFFERWRAPPER_H
#define FRAMEBUFFERWRAPPER_H

#include <QGLFunctions>
#include <QImage>

class FrameBufferWrapper : public QGLFunctions
{
public:
    FrameBufferWrapper(int w, int h);
    void        bind();
    void        release();
    GLuint      texture(){return m_texture;}

    GLubyte*    getData();
    QImage*     genQImage();

    int         width() const {return m_width;}
    int         height() const{return m_height;}

private:
    GLuint      m_texture;
    GLuint      m_fbo;
    GLuint      m_depth_rb;
    int         m_width;
    int         m_height;

    GLuint*     _map;
};

#endif // FRAMEBUFFERWRAPPER_H
