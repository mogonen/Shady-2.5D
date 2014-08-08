#include "framebufferwrapper.h"

FrameBufferWrapper::FrameBufferWrapper(int w, int h)
{
    initializeGLFunctions();
    m_width = w;
    m_height = h;
    //RGBA8 2D texture, 24 bit depth texture, 256x256
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //NULL means reserve texture memory, but texels are undefined
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    //-------------------------
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    //Attach 2D texture to this FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
    //-------------------------
    glGenRenderbuffers(1, &m_depth_rb);
    glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
    //-------------------------
    //Attach depth buffer to FBO
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rb);

    _map = new GLuint[m_width*m_height];
}

void FrameBufferWrapper::bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void FrameBufferWrapper::release()
{
    glReadPixels(0,0,m_width, m_height, GL_RGBA8, GL_UNSIGNED_INT_8_8_8_8, _map);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLubyte* FrameBufferWrapper::getData()
{
    glBindTexture(GL_TEXTURE_2D, m_texture);
    GLubyte* map = new GLubyte[m_width*m_height*4];
    glGetTexImage(GL_TEXTURE_2D,0,GL_RGBA8, GL_UNSIGNED_BYTE, map);
    return map;
}

QImage* FrameBufferWrapper::genQImage()
{
    //glBindTexture(GL_TEXTURE_2D, m_texture);
    //GLuint* map = new GLuint[m_width*m_height];
    //glGetTexImage(GL_TEXTURE_2D,0, GL_RGBA8, GL_UNSIGNED_INT_8_8_8_8, map);
    //swap alpha bits
    int size = m_width*m_height;
    for(int i = 0; i < size; i++)
    {
        unsigned int val = _map[i];
        if (val){
            _map[i] = (val << 24) | (val >> 8); //(val << 24) | (val >> 8);
        }
    }

    return new QImage((uchar*)_map, m_width, m_height, QImage::Format_ARGB32);
}
