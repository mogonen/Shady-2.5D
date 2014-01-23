#ifndef SHADERPARAMETERS_H
#define SHADERPARAMETERS_H

#include <QVector2D>
#include <QVector3D>

class ShaderParameters
{
public:

    ShaderParameters();
    ~ShaderParameters();

    float m_alphaValue;
    float m_translucency;
    QVector3D m_centerDepth;
    int   m_layerLabel;
    float m_SMQuality;
    QVector2D m_averageNormal;
    QVector3D m_trueNormal;
    float m_boundingbox[4];
    bool m_shadowcreator;

};

#endif // SHADERPARAMETERS_H
