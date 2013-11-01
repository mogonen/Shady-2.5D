#include "shaderparameters.h"

ShaderParameters::ShaderParameters()
{
    m_ShaderProgram = NULL;
    m_Dist = 0.0;
    m_Alpha = 1.0;
    m_Width = 1.0;
    m_Height = 1.0;
    m_SM_QUality = 0.5;
    m_toggle_Mirror = false;
    m_SMInitialized = false;
    glEnable(GL_TEXTURE_2D);
    initializeGLFunctions();
    InitializeTextures();
}

ShaderParameters::~ShaderParameters()
{
    glDeleteTextures(1,&m_Dark);
    glDeleteTextures(1,&m_Bright);
    glDeleteTextures(1,&m_ShapeMap);
}


void ShaderParameters::InitializeTextures()
{
    glGenTextures(1,&m_Dark);
    glGenTextures(1,&m_Bright);
    glGenTextures(1,&m_ShapeMap);

    ResetDarkImage();
    ResetBrightImage();

    //set shapemap as null
    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderParameters::ResetDarkImage()
{
    //set dark image as all black
    char temp[4];
    memset(temp,0,3);
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderParameters::ResetBrightImage()
{
    //set birght image as all white
    char temp[4];
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    memset(temp,255,4);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  1, 1,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderParameters::LoadBrightImage(unsigned char *data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderParameters::LoadDarkImage(unsigned char *data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderParameters::GrabShapeMap(int w, int h)
{
    m_Width = w;
    m_Height = h;
    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
    if(!m_SMInitialized)
    {
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (h-w)/2, w, w,0);
        m_SMInitialized = true;
    }
    else
        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (h-w)/2, w, w);
}


void ShaderParameters::SetAllLocalParameters()
{
    //bind all the textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
    glDisable(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glDisable(GL_TEXTURE0+1);

    glActiveTexture(GL_TEXTURE31);
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    glDisable(GL_TEXTURE31);

//    glActiveTexture(GL_TEXTURE0+3);
//    glBindTexture(GL_TEXTURE_2D, m_Dark);
//    glDisable(GL_TEXTURE0+3);

//    glActiveTexture(GL_TEXTURE0+4);
//    glBindTexture(GL_TEXTURE_2D, m_Dark);
//    glDisable(GL_TEXTURE0+4);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);

    //set the local parameters
    m_ShaderProgram->setUniformValue("dist", (float)m_Dist);
    m_ShaderProgram->setUniformValue("alpha", (float)m_Alpha);
    m_ShaderProgram->setUniformValue("width", (float)m_Width);
    m_ShaderProgram->setUniformValue("height", (float)m_Height);
    m_ShaderProgram->setUniformValue("LOD", (float)m_LOD);
    m_ShaderProgram->setUniformValue("toggle_Mirror", m_toggle_Mirror);
    m_ShaderProgram->setUniformValue("SM_Quality", m_SM_QUality);
}
