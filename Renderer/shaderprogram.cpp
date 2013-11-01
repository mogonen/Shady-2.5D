#include "shaderprogram.h"
#include <QFileInfo>
#include <QMessageBox>

ShaderProgram::ShaderProgram()
{
    m_VShaderFile = QString(":Basic.vsh");
    m_FShaderFile = QString(":Basic.fsh");
//    m_VShaderFile = QString("/Renderer/Basic.vsh");
//    m_FShaderFile = QString("/Renderer/Basic.fsh");
    m_VertexShader = NULL;
    m_FragmentShader = NULL;
}

ShaderProgram::ShaderProgram(const QString &VFile, const QString &FFile) : m_VShaderFile(VFile), m_FShaderFile(FFile)
{
    m_VertexShader = NULL;
    m_FragmentShader = NULL;
}

void ShaderProgram::GrabResultsTes()
{
//    glBindTexture(GL_TEXTURE_2D, m_resTexture);
//    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (h-w)/2, w, w,0);
}

void ShaderProgram::Initialize()
{
    LoadShader();

    glGenTextures(1,&m_resTexture);
    glBindTexture(GL_TEXTURE_2D, m_resTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );


    this->setUniformValue("tex_SM", 0);
    this->setUniformValue("tex_DI_Dark", 1);
    this->setUniformValue("tex_DI_Bright", 31);
    this->setUniformValue("tex_BG", 3);
    this->setUniformValue("tex_env", 4);
    this->setUniformValue("is_Video", 0);
    this->setUniformValue("dist", (float)0.0);
    this->setUniformValue("alpha", (float)1.0);
    this->setUniformValue("filter_size", (float)1.0);
    this->setUniformValue("amb_strength", (float)0.25);
    this->setUniformValue("width", (float)1.0);
    this->setUniformValue("height", (float)1.0);
    this->setUniformValue("LOD", (float)0.0);
    this->setUniformValue("toggle_ShaAmbCos", (int)7);
    this->setUniformValue("toggle_Mirror", false);
    this->setUniformValue("toggle_Point", false);
    this->setUniformValue("SM_Quality", (float)0.5);
    this->setUniformValue("Cartoon_sha", (float)0.5);
    this->setUniformValue("light_dir", -QVector3D(0,0.0,1.0));
}


void ShaderProgram::LoadShader(const QString& vshader,const QString& fshader)
{
    if(!vshader.isEmpty())
    {
        m_VShaderFile = vshader;
        m_FShaderFile = fshader;
    }

//    this->release();
//    this->removeAllShaders();

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
        QMessageBox::critical(0, "GLSL Shader Program Linker Error",
                              QString("GLSL ")+this->log());
    }
    else this->bind();
}


void ShaderProgram::AddParamSet(ShaderParameters *p)
{
    m_ShaderParamList.push_back(p);
    p->AssociateToProgram(this);
}

void ShaderProgram::RemoveParamSet(ShaderParameters *p)
{
    m_ShaderParamList.removeOne(p);
    LoadAllParamSet();
}

void ShaderProgram::LoadAllParamSet()
{
    ParamList::Iterator it;
    for(it = m_ShaderParamList.begin();it!=m_ShaderParamList.end();it++)
    {
        LoadParamSet(*it);
    }
}

void ShaderProgram::LoadParamSet(ShaderParameters *p)
{
    p->SetAllLocalParameters();
}






