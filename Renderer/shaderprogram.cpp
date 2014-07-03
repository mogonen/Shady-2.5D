#include "shaderprogram.h"
#include <QFileInfo>
#include <QMessageBox>


ShaderProgram::ShaderProgram(SHADER_TYPE type)
{

    switch(type)
    {
    case(TYPE_RENDER):
//        m_VShaderFile = QString(":Basic.vsh");
//        m_FShaderFile = QString(":Basic.fsh");
        m_VShaderFile = QString(".//../Shady-2.5D/Renderer/Basic.vsh");
        m_FShaderFile = QString(".//../Shady-2.5D/Renderer/Basic.fsh");
        break;
    case(TYPE_MODEL):
//        m_VShaderFile = QString(":Basic.vsh");
//        m_FShaderFile = QString(":Modelling.fsh");
        m_VShaderFile = QString(":Basic.vsh");
        m_FShaderFile = QString(":Modelling.fsh");
        break;
    case(TYPE_COMPOSITE):
//        m_VShaderFile = QString(":Basic.vsh");
//        m_FShaderFile = QString(":Modelling.fsh");
        m_VShaderFile = QString(".//../Shady-2.5D/Renderer/Basic.vsh");
        m_FShaderFile = QString(".//../Shady-2.5D/Renderer/Composite.fsh");
        break;
    }
    m_VertexShader = NULL;
    m_FragmentShader = NULL;
    m_type = type;
}

ShaderProgram::ShaderProgram(const QString &VFile, const QString &FFile) : m_VShaderFile(VFile), m_FShaderFile(FFile)
{
    m_VertexShader = NULL;
    m_FragmentShader = NULL;
}

ShaderProgram::~ShaderProgram()
{
//    glDeleteTextures(1,&m_Dark);
//    glDeleteTextures(1,&m_Bright);
//    glDeleteTextures(1,&m_ShapeMap);
//    glDeleteTextures(1, &m_BG);
//    glDeleteTextures(1, &m_LD);
    if(m_DarkFBO)
        delete m_DarkFBO;
    if(m_BrightFBO)
        delete m_BrightFBO;
    if(m_ShapeMapFBO)
        delete m_ShapeMapFBO;
    if(m_LDFBO)
        delete m_LDFBO;
    if(m_ShadeFBO)
        delete m_ShadeFBO;
    if(m_DispFBO)
        delete m_DispFBO;
}

void ShaderProgram::GrabResultsTes()
{
    //    glBindTexture(GL_TEXTURE_2D, m_resTexture);
    //    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (h-w)/2, w, w,0);
}

void ShaderProgram::Initialize()
{
    m_isInitialized = true;
    initializeGLFunctions();
    LoadShader();

//    glGenTextures(1,&m_resTexture);
//    glBindTexture(GL_TEXTURE_2D, m_resTexture);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  0, 0,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//    InitializeTextures();
    glGenTextures(1,&m_BG);
    glGenTextures(1,&m_Env);
    InitializeParameters();
    SetParametersToShader();
}

void ShaderProgram::InitializeParameters()
{
    m_Dist = 0.0;
    m_Alpha = 1.0;
    m_filter_size = 1.0;
    m_amb_strength = 0.25;
    m_Cartoon_sha = 0.5;

    m_Width = 1.0;
    m_Height = 1.0;
    m_SM_Quality = 0.5;
    m_surface_disp = 0.1;

    m_toggle_Mirror = false;
    m_toggle_Point = true;

//    m_SMInitialized = false;
//    m_DarkInitialized = false;
//    m_BrightInitialized = false;
//    m_LDInitialized = false;
//    m_SDInitialized = false;

    m_DarkFBO = NULL;
    m_BrightFBO = NULL;
    m_ShapeMapFBO = NULL;
    m_LDFBO = NULL;
    m_ShadeFBO = NULL;
    m_DispFBO = NULL;
    m_isFBOInitialized = false;
    m_toggle_ShaAmbCos = 7;
}

void ShaderProgram::SetParametersToShader()
{
    this->setUniformValue("tex_SM", 0);
    this->setUniformValue("tex_LD", 1);
    this->setUniformValue("tex_DI_Dark", 2);
    this->setUniformValue("tex_DI_Bright", 3);
    this->setUniformValue("tex_BG", 4);
    this->setUniformValue("tex_Env", 5);
    this->setUniformValue("tex_Shade", 6);
    this->setUniformValue("tex_Disp", 7);
    this->setUniformValue("is_Video", 0);


    this->setUniformValue("filter_size", (float)m_filter_size);
    this->setUniformValue("amb_strength", (float)m_amb_strength);
    this->setUniformValue("width", (float)m_Width);
    this->setUniformValue("height", (float)m_Height);

    this->setUniformValue("toggle_Point", m_toggle_Point);
    this->setUniformValue("Cartoon_sha", (float)m_Cartoon_sha);
    this->setUniformValue("surface_disp", m_surface_disp);

    this->setUniformValue("light_dir", m_MousePos);

    this->setUniformValue("dist", (float)m_Dist);
    this->setUniformValue("alpha", (float)m_Alpha);

    this->setUniformValue("LOD", (float)m_LOD);
    this->setUniformValue("toggle_Mirror", m_toggle_Mirror);
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);

    this->setUniformValue("SM_Quality", (float)m_SM_Quality);

}


void ShaderProgram::initialiFBO(int w, int h)
{
    if(m_DarkFBO)
    {
        m_DarkFBO->release();
        delete m_DarkFBO;
    }
    m_DarkFBO = new FrameBufferWrapper(w,h);

    if(m_BrightFBO)
    {
        m_BrightFBO->release();
        delete m_BrightFBO;
    }
    m_BrightFBO = new FrameBufferWrapper(w,h);

    if(m_ShapeMapFBO)
    {
        m_ShapeMapFBO->release();
        delete m_ShapeMapFBO;
    }
    m_ShapeMapFBO = new FrameBufferWrapper(w,h);

    if(m_LDFBO)
    {
        m_LDFBO->release();
        delete m_LDFBO;
    }
    m_LDFBO = new FrameBufferWrapper(w,h);

    if(m_ShadeFBO)
    {
        m_ShadeFBO->release();
        delete m_ShadeFBO;
    }
    m_ShadeFBO = new QGLFramebufferObject(w,h);


    if(m_DispFBO)
    {
        m_DispFBO->release();
        delete m_DispFBO;
    }
    m_DispFBO = new FrameBufferWrapper(w,h);

    m_isFBOInitialized = true;
}

void ShaderProgram::BindAllFBO()
{
    if(m_DarkFBO)
        m_DarkFBO->bind();
    if(m_BrightFBO)
        m_BrightFBO->bind();
    if(m_ShapeMapFBO)
        m_ShapeMapFBO->bind();
    if(m_LDFBO)
        m_LDFBO->bind();
    if(m_ShadeFBO)
        m_ShadeFBO->bind();
    if(m_DispFBO)
        m_DispFBO->bind();
}

void ShaderProgram::ReleaseAllFBO()
{
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if(m_DarkFBO)
        m_DarkFBO->release();
    if(m_BrightFBO)
        m_BrightFBO->release();
    if(m_ShapeMapFBO)
        m_ShapeMapFBO->release();
    if(m_LDFBO)
        m_LDFBO->release();
    if(m_ShadeFBO)
        m_ShadeFBO->release();
    if(m_DispFBO)
        m_DispFBO->release();
}

void ShaderProgram::ReloadShader()
{
    LoadShader();
    SetParametersToShader();
}

void ShaderProgram::LoadShader(const QString& vshader,const QString& fshader)
{
    if(this->isLinked())
    {
        this->release();
        this->removeAllShaders();
    }

    if(!vshader.isEmpty())
    {
        m_VShaderFile = vshader;
        m_FShaderFile = fshader;
    }

    if(m_VertexShader)
    {
        delete m_VertexShader;
        m_VertexShader = NULL;
    }

    if(m_FragmentShader)
    {
        delete m_FragmentShader;
        m_FragmentShader = NULL;
    }

    // load and compile vertex shader
    QFileInfo vsh(m_VShaderFile);
    if(vsh.exists())
    {
        m_VertexShader = new QGLShader(QGLShader::Vertex);
        if(m_VertexShader->compileSourceFile(m_VShaderFile))
            this->addShader(m_VertexShader);
        else
            QMessageBox::critical(0, "GLSL Vertex Shader Error",
                                  QString("GLSL ")+m_VertexShader->log());
    }
    else qWarning() << "Vertex Shader source file " << m_VShaderFile << " not found.";


    // load and compile fragment shader
    QFileInfo fsh(m_FShaderFile);
    if(fsh.exists())
    {
        m_FragmentShader = new QGLShader(QGLShader::Fragment);
        if(m_FragmentShader->compileSourceFile(m_FShaderFile))
            this->addShader(m_FragmentShader);
        else
            QMessageBox::critical(0, "GLSL Fragment Shader Error",
                                  QString("GLSL ")+m_FragmentShader->log());
    }
    else qWarning() << "Fragment Shader source file " << m_FShaderFile << " not found.";

    if(!this->link())
    {
        QMessageBox::critical(0, "GLSL Shader Program Linker Error", QString("GLSL ")+this->log());
    }
    else this->bind();

}


//void ShaderProgram::AddParamSet(ShaderParameters *p)
//{
//    m_ShaderParamList.push_back(p);
//    p->AssociateToProgram(this);
//}

//void ShaderProgram::RemoveParamSet(ShaderParameters *p)
//{
//    m_ShaderParamList.removeOne(p);
//    LoadAllParamSet();
//}

//void ShaderProgram::LoadAllParamSet()
//{
//    ParamList::Iterator it;
//    for(it = m_ShaderParamList.begin();it!=m_ShaderParamList.end();it++)
//    {
//        LoadParamSet(*it);
//    }
//}

//void ShaderProgram::LoadParamSet(ShaderParameters *p)
//{
//    //    p->SetAllLocalParameters();
//}

//void ShaderProgram::InitializeTextures()
//{
//    glGenTextures(1,&m_Dark);
//    glGenTextures(1,&m_Bright);
//    glGenTextures(1,&m_ShapeMap);
//    glGenTextures(1,&m_BG);
//    glGenTextures(1,&m_LD);
//    glGenTextures(1,&m_Shade);
//    glGenFramebuffers(1, &m_FBO);
//    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

//    ResetDarkImage();
//    ResetBrightImage();
//    ResetLDImage();

//    //    //set background as black
//    //    char temp[4];
//    //    memset(temp,255,3);
//    //    temp[3] = 255;
//    //    glBindTexture(GL_TEXTURE_2D, m_BG);
//    //    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//    //                  1, 1,
//    //                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
//    //    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    //    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//    //set shapemap as null
//    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  0, 0,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

//}

void ShaderProgram::SetTextureToShader()
{
//    m_ShapeMapFBO->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ShapeMapFBO->texture());
    glDisable(GL_TEXTURE0);
//    m_ShapeMapFBO->release();


    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, m_LDFBO->texture());
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glDisable(GL_TEXTURE0+1);


//    m_DarkFBO->bind();
    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, m_DarkFBO->texture());
    glDisable(GL_TEXTURE0+2);
//    m_DarkFBO->release();

//    m_BrightFBO->bind();
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, m_BrightFBO->texture());
    glDisable(GL_TEXTURE0+3);
//    m_BrightFBO->release();

    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, m_BG);
    glDisable(GL_TEXTURE0+4);


    glActiveTexture(GL_TEXTURE0+5);
    glBindTexture(GL_TEXTURE_2D, m_Env);
    glDisable(GL_TEXTURE0+5);

    glActiveTexture(GL_TEXTURE0+6);
    glBindTexture(GL_TEXTURE_2D, m_ShadeFBO->texture());
    glDisable(GL_TEXTURE0+6);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE0+7);
    glBindTexture(GL_TEXTURE_2D, m_DispFBO->texture());
    glDisable(GL_TEXTURE0+7);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);
}

//void ShaderProgram::ResetDarkImage()
//{
//    //set dark image as all black
//    char temp[4];
//    memset(temp,0,3);
//    temp[3] = 255;
//    glBindTexture(GL_TEXTURE_2D, m_Dark);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  1, 1,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//}

//void ShaderProgram::ResetBrightImage()
//{
//    //set birght image as all white
//    char temp[4];
//    glBindTexture(GL_TEXTURE_2D, m_Bright);
//    memset(temp,255,4);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  1, 1,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//}

//void ShaderProgram::ResetLDImage()
//{
//    //set LD image as all black
//    float temp[4];
//    memset(temp,0.5,3);
//    temp[3] = 1;
//    glBindTexture(GL_TEXTURE_2D, m_LD);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  1, 1,
//                  0, GL_RGBA, GL_FLOAT, temp);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//}

//void ShaderProgram::ResetShadeImage()
//{
//    //set birght image as all white
//    char temp[4];
//    glBindTexture(GL_TEXTURE_2D, m_Shade);
//    memset(temp,255,4);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  1, 1,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//}

//void ShaderProgram::LoadBrightImage(unsigned char *data, int width, int height)
//{
//    glBindTexture(GL_TEXTURE_2D, m_Bright);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  width, height,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//}

//void ShaderProgram::LoadDarkImage(unsigned char *data, int width, int height)
//{
//    glBindTexture(GL_TEXTURE_2D, m_Dark);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  width, height,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//}

void ShaderProgram::LoadBGImage(unsigned char *data, int width, int height)
{
    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, m_BG);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDisable(GL_TEXTURE0+4);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);
}

void ShaderProgram::LoadEnvImage(unsigned char *data, int width, int height)
{
    glActiveTexture(GL_TEXTURE0+5);
    glBindTexture(GL_TEXTURE_2D, m_Env);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDisable(GL_TEXTURE0+5);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);
}


//void ShaderProgram::LoadBGImage(unsigned char *data, int width, int height)
//{
//    glActiveTexture(GL_TEXTURE0+3);
//    glBindTexture(GL_TEXTURE_2D, m_BG);
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
//                  width, height,
//                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
//    glDisable(GL_TEXTURE0+3);

//    glActiveTexture(GL_TEXTURE0);
//    glDisable(GL_TEXTURE0);
//}


//void ShaderProgram::GrabShapeMap(int w, int h)
//{
//    m_Width = w;
//    m_Height = h;

//    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
//    if(!m_SMInitialized)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (h-w)/2, w, w,0);
//        m_SMInitialized = true;
//    }
//    else
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (h-w)/2, w, w);
//}


//void ShaderProgram::GrabDarkMap()
//{
//    glBindTexture(GL_TEXTURE_2D, m_Dark);
//    if(!m_DarkInitialized)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
//        m_DarkInitialized = true;
//    }
//    else
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
//}

//void ShaderProgram::GrabBrightMap()
//{
//    glBindTexture(GL_TEXTURE_2D, m_Bright);
//    if(!m_BrightInitialized)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
//        m_BrightInitialized = true;
//    }
//    else
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
//}

//void ShaderProgram::GrabLDMap()
//{
//    glBindTexture(GL_TEXTURE_2D, m_LD);
//    if(!m_LDInitialized)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
//        m_LDInitialized = true;
//    }
//    else
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
//}

//void ShaderProgram::GrabShadeMap()
//{
//    glBindTexture(GL_TEXTURE_2D, m_Shade);
//    if(!m_SDInitialized)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
//        m_SDInitialized = true;
//    }
//    else
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);


//}

//void ShaderProgram::SetShadeMapFBO()
//{
//    QGLFramebufferObject m_FBO;
//    m_FBO.texture();
//    glBindFramebuffer(GL_TEXTURE_2D, m_Shade);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, m_Shade, 0);
//}


void ShaderProgram::SetDepthValue(double dep)
{
    m_Dist = dep;
    this->setUniformValue("dist", m_Dist);
}
void ShaderProgram::SetAlphaValue(double alp)
{
    m_Alpha = alp;
    this->setUniformValue("alpha", m_Alpha);
}


void ShaderProgram::SetFilterValue(double filter)
{
    m_filter_size = filter;
    this->setUniformValue("filter_size", m_filter_size);
}
void ShaderProgram::SetAmbValue(double amb)
{
    m_amb_strength = amb;
    //    ShaderProgram->bind();
    this->setUniformValue("amb_strength", m_amb_strength);
    //    qDebug()<<dep;
    //    LoadShader("./GLSL/Basic.vsh", "./GLSL/Basic.fsh");

}

void ShaderProgram::SetLevelOfDetail(double LOD)
{
    m_LOD = LOD;
    this->setUniformValue("LOD", m_LOD);

}

void ShaderProgram::SetSMQuality(double Quality)
{
    m_SM_Quality = Quality;
    this->setUniformValue("SM_Quality", m_SM_Quality);

}

void ShaderProgram::SetCartoonSha(double Strength)
{
    m_Cartoon_sha = Strength;
    //    qDebug()<<m_CartoonSha;
    this->setUniformValue("Cartoon_sha", m_Cartoon_sha);

}
void ShaderProgram::SetSurfDisp(double Disp)
{
    m_surface_disp = Disp;
    //    qDebug()<<m_CartoonSha;
    this->setUniformValue("surface_disp", m_surface_disp);

}


void ShaderProgram::ToggleCos(bool info)
{
    m_toggle_ShaAmbCos &= 0x1e;
    if(info)
        m_toggle_ShaAmbCos += 1;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleAmb(bool info)
{
    m_toggle_ShaAmbCos &= 0x1d;
    if(info)
        m_toggle_ShaAmbCos += 2;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleSha(bool info)
{
    m_toggle_ShaAmbCos &= 0x1b;
    if(info)
        m_toggle_ShaAmbCos += 4;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}


void ShaderProgram::ToggleNormal(bool info)
{
    m_toggle_ShaAmbCos &= 0x17;
    if(info)
        m_toggle_ShaAmbCos += 8;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleCenter(bool info)
{
    m_toggle_ShaAmbCos &= 0xf;
    if(info)
        m_toggle_ShaAmbCos += 16;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleMirror(bool info)
{
    m_toggle_Mirror = info;
    this->setUniformValue("toggle_Mirror", m_toggle_Mirror);
}

void ShaderProgram::TogglePoint(bool info)
{
    m_toggle_Point = info;
    this->setUniformValue("toggle_Point", m_toggle_Point);
}

void ShaderProgram::SetCurTex(int index)
{
    m_cur_tex = index;
    this->setUniformValue("cur_tex", m_cur_tex);
}

void ShaderProgram::SetLightPos(QVector3D light_pos)
{
    m_MousePos = light_pos;
    this->setUniformValue("light_dir", m_MousePos);
}

void ShaderProgram::LoadShaperParameters(ShapeList Shapes)
{
    float refvalue[10];
    int reflToggled[10];
    QVector3D normalvalue[10];
    QVector3D centerDepth[10];
    float boundingbox[40];
    int shadowcreator[10];
//    for(int i=0;i<10;i++)
//        normalvalue[i] = QVector3D(0.0,0.0,0.0);

    //position 0 is (0,0,0),
    //since shader takes 0 as background
    int m = 1;

    FOR_ALL_CONST_ITEMS(ShapeList, Shapes)
    {
        ShaderParameters Param = (*it)->getShaderParam();
        Shape_p pShape = (*it);
        refvalue[m]     = Param.m_alphaValue;
        reflToggled[m]  = Param.m_reflectToggled;

//      normalvalue[m] = QVector3D(Param.m_trueNormal/2,0.0)+QVector3D(0.5,0.5,0.0);

        //----> Here I pass the values from the shape parameters to the array
        Vec3 n3d        = pShape->value[NORMAL_CHANNEL];
        normalvalue[m]  = QVector3D(n3d.x, n3d.y, n3d.z); //Param.m_trueNormal;
        centerDepth[m]  = QVector3D(pShape->P().x, pShape->P().y, pShape->m_assignedDepth); //pShape->m_assignedDepth; //Param.m_centerDepth
        shadowcreator[m] = true;//Param.m_shadowcreator;

        qDebug()<<"passed to shader center"<<m<<centerDepth[m];
        qDebug()<<"passed to shader normal"<<m<<normalvalue[m];
        BBox bbox;
        (*it)->getBBox(bbox);
        Point piv = bbox.pivot();
        Vec2 diag = bbox.diag();
        double bb[4] = {piv.x, piv.y, diag.x, diag.y};
        for(int i=0;i<4;i++)
            boundingbox[m*4+i] = bb[i];//Param.m_boundingbox[i];
        m++;
    }

    this->setUniformValueArray("refValues", refvalue, 10,1);
    this->setUniformValueArray("normalValues", normalvalue, 10);
    this->setUniformValueArray("centerDepth", centerDepth, 10);
    this->setUniformValueArray("boundingbox", boundingbox, 40,1);
    this->setUniformValueArray("shadowcreator", shadowcreator,10);
    this->setUniformValueArray("refToggled", reflToggled,10);

}

