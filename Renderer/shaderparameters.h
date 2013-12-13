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



//    void SetSMTexture(QImage img, int option = 3);
//    void SetDarkTexture(QImage img, int option = 3);
//    void SetBrightTexture(QImage img, int option = 3);

//    void SetDepthValue(double dep);
//    void SetAlphaValue(double alp);
//    void SetLevelOfDetail(double LOD);
//    void SetSMQuality(double Quality);
//    void ToggleMirror(bool info);

    ShaderProgram *m_ShaderProgram;




};

#endif // SHADERPARAMETERS_H
