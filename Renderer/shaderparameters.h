#ifndef SHADERPARAMETERS_H
#define SHADERPARAMETERS_H

#include <QVector3D>

class ShaderParameters
{
public:

    ShaderParameters();
    ~ShaderParameters();

    float m_alphaValue;
    float m_translucency;
    float m_centerDepth;
    int   m_layerLabel;
    float m_SMQuality;
    QVector3D m_averageNormal;


};

#endif // SHADERPARAMETERS_H
