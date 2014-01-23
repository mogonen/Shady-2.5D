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
    case(TYPE_AMB):
//        m_VShaderFile = QString(":Basic.vsh");
//        m_FShaderFile = QString(":Modelling.fsh");
        m_VShaderFile = QString(".//../Shady-2.5D/Renderer/Basic.vsh");
        m_FShaderFile = QString(".//../Shady-2.5D/Renderer/Ambient.fsh");
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
    glDeleteTextures(1,&m_Dark);
    glDeleteTextures(1,&m_Bright);
    glDeleteTextures(1,&m_ShapeMap);
    glDeleteTextures(1, &m_BG);
    glDeleteTextures(1, &m_LD);

}


void ShaderProgram::GrabResultsTes()
{
    //    glBindTexture(GL_TEXTURE_2D, m_resTexture);
    //    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (h-w)/2, w, w,0);
}

void ShaderProgram::Initialize()
{
    initializeGLFunctions();
    LoadShader();

    glGenTextures(1,&m_resTexture);
    glBindTexture(GL_TEXTURE_2D, m_resTexture);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    InitializeTextures();
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
    m_surface_disp = 0.01;

    m_toggle_Mirror = false;
    m_toggle_Point = true;

    m_SMInitialized = false;
    m_DarkInitialized = false;
    m_BrightInitialized = false;
    m_LDInitialized = false;
}

void ShaderProgram::SetParametersToShader()
{
    this->setUniformValue("tex_SM", 0);
    this->setUniformValue("tex_DI_Dark", 1);
    this->setUniformValue("tex_DI_Bright", 2);
    this->setUniformValue("tex_BG", 3);
    this->setUniformValue("tex_LD", 4);
    this->setUniformValue("tex_Env", 5);

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
    this->setUniformValue("SM_Quality", (float)m_SM_Quality);

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
        QMessageBox::critical(0, "GLSL Shader Program Linker Error",
                              QString("GLSL ")+this->log());
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

void ShaderProgram::InitializeTextures()
{
    glGenTextures(1,&m_Dark);
    glGenTextures(1,&m_Bright);
    glGenTextures(1,&m_ShapeMap);
    glGenTextures(1,&m_BG);
    glGenTextures(1,&m_LD);

    ResetDarkImage();
    ResetBrightImage();
    ResetLDImage();

    //    //set background as black
    //    char temp[4];
    //    memset(temp,255,3);
    //    temp[3] = 255;
    //    glBindTexture(GL_TEXTURE_2D, m_BG);
    //    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
    //                  1, 1,
    //                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
    //    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    //    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    //set shapemap as null
    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

}

void ShaderProgram::SetTextureToShader()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ShapeMap);
    glDisable(GL_TEXTURE0);

    glActiveTexture(GL_TEXTURE0+1);
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glDisable(GL_TEXTURE0+1);

    glActiveTexture(GL_TEXTURE0+2);
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    glDisable(GL_TEXTURE0+2);


    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, m_BG);
    glDisable(GL_TEXTURE0+3);

    glActiveTexture(GL_TEXTURE0+4);
    glBindTexture(GL_TEXTURE_2D, m_LD);
    glDisable(GL_TEXTURE0+4);

    glActiveTexture(GL_TEXTURE0+5);
    glBindTexture(GL_TEXTURE_2D, m_env);
    glDisable(GL_TEXTURE0+5);

    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);
}

void ShaderProgram::ResetDarkImage()
{
    //set dark image as all black
    char temp[4];
    memset(temp,0,3);
    temp[3] = 255;
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  1, 1,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, temp);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderProgram::ResetBrightImage()
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

void ShaderProgram::ResetLDImage()
{
    //set LD image as all black
    float temp[4];
    memset(temp,0.5,3);
    temp[3] = 1;
    glBindTexture(GL_TEXTURE_2D, m_LD);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  1, 1,
                  0, GL_RGBA, GL_FLOAT, temp);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
}


void ShaderProgram::LoadBrightImage(unsigned char *data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderProgram::LoadDarkImage(unsigned char *data, int width, int height)
{
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void ShaderProgram::LoadBGImage(unsigned char *data, int width, int height)
{
    glActiveTexture(GL_TEXTURE0+3);
    glBindTexture(GL_TEXTURE_2D, m_BG);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  width, height,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glDisable(GL_TEXTURE0+3);

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


void ShaderProgram::GrabShapeMap(int w, int h)
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


void ShaderProgram::GrabDarkMap()
{
    glBindTexture(GL_TEXTURE_2D, m_Dark);
    if(!m_DarkInitialized)
    {
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
        m_DarkInitialized = true;
    }
    else
        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
}

void ShaderProgram::GrabBrightMap()
{
    glBindTexture(GL_TEXTURE_2D, m_Bright);
    if(!m_BrightInitialized)
    {
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
        m_BrightInitialized = true;
    }
    else
        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
}

void ShaderProgram::GrabLDMap()
{
    glBindTexture(GL_TEXTURE_2D, m_LD);
    if(!m_LDInitialized)
    {
        glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, (m_Height-m_Width)/2, m_Width, m_Width,0);
        m_LDInitialized = true;
    }
    else
        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0, (m_Height-m_Width)/2, m_Width, m_Width);
}



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
    m_toggle_ShaAmbCos &= 0xe;
    if(info)
        m_toggle_ShaAmbCos += 1;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleAmb(bool info)
{
    m_toggle_ShaAmbCos &= 0xd;
    if(info)
        m_toggle_ShaAmbCos += 2;
    this->setUniformValue("toggle_ShaAmbCos", (int)m_toggle_ShaAmbCos);
}

void ShaderProgram::ToggleSha(bool info)
{
    m_toggle_ShaAmbCos &= 0xb;
    if(info)
        m_toggle_ShaAmbCos += 4;
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
    qDebug()<<"Mouse pos in shader"<<m_MousePos;
    this->setUniformValue("light_dir", m_MousePos);
}

void ShaderProgram::LoadShaperParameters(ShapeList Shapes)
{
    float refValues[10];
    QVector3D normalValues[10];
    QVector3D centerDepth[10];
    float boundingbox[40];
    int shadowcreator[10];
//    for(int i=0;i<10;i++)
//        normalValues[i] = QVector3D(0.0,0.0,0.0);

    //position 0 is (0,0,0),
    //since shader takes 0 as background
    int m=1;
    FOR_ALL_CONST_ITEMS(ShapeList, Shapes){
        ShaderParameters Param = (*it)->getShaderParam();
        refValues[m] = Param.m_alphaValue;
//        normalValues[m] = QVector3D(Param.m_trueNormal/2,0.0)+QVector3D(0.5,0.5,0.0);
        normalValues[m] = Param.m_trueNormal;
        centerDepth[m] = Param.m_centerDepth;
        shadowcreator[m] = Param.m_shadowcreator;
        qDebug()<<"passed to shader center"<<m<<centerDepth[m];
        qDebug()<<"passed to shader normal"<<m<<normalValues[m];
        for(int i=0;i<4;i++)
            boundingbox[m*4+i] = Param.m_boundingbox[i];
        m++;
    }
    this->setUniformValueArray("refValues", refValues, 10,1);
    this->setUniformValueArray("normalValues", normalValues, 10);
    this->setUniformValueArray("centerDepth", centerDepth, 10);
    this->setUniformValueArray("boundingbox", boundingbox, 40,1);
    this->setUniformValueArray("shadowcreator",shadowcreator,10);
}

