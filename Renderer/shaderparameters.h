#ifndef SHADERPARAMETERS_H
#define SHADERPARAMETERS_H

#include <QImage>
#include <QGLFunctions>
#include "shaderprogram.h"

//typedef Vec4 RGBA;
//typedef Vec3 RGB;

class ShaderProgram;

class ShaderParameters : public QGLFunctions
{
public:

    ShaderParameters();
    ~ShaderParameters();

    void AssociateToProgram(ShaderProgram* p){m_ShaderProgram = p;}


    void SetAllLocalParameters();
    void ResetDarkImage();
    void ResetBrightImage();
    void LoadDarkImage(unsigned char *data, int width, int height);
    void LoadBrightImage(unsigned char *data, int width, int height);
    void InitializeTextures();
    void GrabShapeMap(int w, int h);

//    void SetSMTexture(QImage img, int option = 3);
//    void SetDarkTexture(QImage img, int option = 3);
//    void SetBrightTexture(QImage img, int option = 3);

//    void SetDepthValue(double dep);
//    void SetAlphaValue(double alp);
//    void SetLevelOfDetail(double LOD);
//    void SetSMQuality(double Quality);
//    void ToggleMirror(bool info);

    ShaderProgram *m_ShaderProgram;
    GLuint m_Dark;
    GLuint m_Bright;
    GLuint m_ShapeMap;

    float m_Width;
    float m_Height;

    float m_Dist;
    float m_Alpha;
    float m_LOD;
    float m_SM_QUality;
    bool m_toggle_Mirror;
    //make sure glCopyTextue is called before subTex is called
    bool m_SMInitialized;
};

#endif // SHADERPARAMETERS_H
