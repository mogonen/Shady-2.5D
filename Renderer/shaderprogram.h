#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QGLShaderProgram>
#include <QList>
#include "shaderparameters.h"

class ShaderParameters;
typedef QList<ShaderParameters *> ParamList;

class ShaderProgram : public QGLShaderProgram
{
public:
    ShaderProgram();
    ShaderProgram(const QString &VFile, const QString &FFile);
    void GrabResultsTes();
    void Initialize();
    void LoadShader(const QString& vshader = QString(),const QString& fshader = QString());
    static GLuint SetTexture(QPixmap TextureMap, bool isMipmap, bool isReversed);
//    void GetMyTexture();
//    void SetDepthValue(double dep);
//    void SetAlphaValue(double alp);
//    void SetAmbValue(double amb);
//    void SetFilterValue(double filter);
//    void SetLevelOfDetail(double LOD);
//    void SetSMQuality(double Quality);
//    void SetCartoonSha(double Strength);

//    void ReloadShader();
////    void ToggleInfor(bool info){m_ShowInfo = info;updateGL();}
//    void ToggleCos(bool info);
//    void ToggleAmb(bool info);
//    void ToggleSha(bool info);
//    void ToggleMirror(bool info);
//    void TogglePoint(bool info);

//    void SetSMTexture(QPixmap, int option = 3);
//    void SetDIDarkTexture(QPixmap, int option = 3);
//    void SetDIBrightTexture(QPixmap, int option = 3);
//    void SetFGTexture(QPixmap, int option = 3);
//    void SetEnvTexture(QPixmap img, int option = 3);

    void AddParamSet(ShaderParameters *p);
    void RemoveParamSet(ShaderParameters *p);
    void LoadAllParamSet();
    void LoadParamSet(ShaderParameters *p);
    bool isInitialized(){return m_isInitialized;}
    void SetInitialized(bool v){m_isInitialized = v;}
private:
    ParamList m_ShaderParamList;
    QGLShader *m_VertexShader;
    QGLShader *m_FragmentShader;
    QString m_VShaderFile;
    QString m_FShaderFile;
    GLuint m_resTexture;
    bool m_isInitialized;

};

#endif // SHADERPROGRAM_H
