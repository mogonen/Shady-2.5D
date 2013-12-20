#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QList>
#include "shaderparameters.h"
#include "shape.h"

class shape;
class ShaderProgram : public QGLShaderProgram, public QGLFunctions
{
public:
    enum SHADER_TYPE {TYPE_RENDER, TYPE_MODEL};
    ShaderProgram(SHADER_TYPE type = TYPE_RENDER);
    ShaderProgram(const QString &VFile, const QString &FFile);
    ~ShaderProgram();

    void GrabResultsTes();
    void Initialize();
    void InitializeParameters();
    void LoadShader(const QString& vshader = QString(),const QString& fshader = QString());
    static GLuint SetTexture(QPixmap TextureMap, bool isMipmap, bool isReversed);

    void SetTextureToShader();
    void ResetDarkImage();
    void ResetBrightImage();
    void ResetLDImage();
    void LoadDarkImage(unsigned char *data, int width, int height);
    void LoadBrightImage(unsigned char *data, int width, int height);
    void LoadBGImage(unsigned char *data, int width, int height);

    void InitializeTextures();
    void GrabShapeMap(int w, int h);
    void GrabDarkMap();
    void GrabBrightMap();
    void GrabLDMap();


    void SetDepthValue(double dep);
    void SetAlphaValue(double alp);
    void SetAmbValue(double amb);
    void SetFilterValue(double filter);
    void SetLevelOfDetail(double LOD);
    void SetSMQuality(double Quality);
    void SetCartoonSha(double Strength);

//    void ToggleInfor(bool info){m_ShowInfo = info;updateGL();}
    void ToggleCos(bool info);
    void ToggleAmb(bool info);
    void ToggleSha(bool info);
    void ToggleMirror(bool info);
    void TogglePoint(bool info);
    void SetCurTex(int index);
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


//    void AddParamSet(ShaderParameters *p);
//    void RemoveParamSet(ShaderParameters *p);
//    void LoadAllParamSet();
//    void LoadParamSet(ShaderParameters *p);
    bool isInitialized(){return m_isInitialized;}
    void SetInitialized(bool v){m_isInitialized = v;}
    void LoadShaperParameters(ShapeList Shapes);

//    void LoadShaperParameters(ShapeList Shapes);

    float m_Width;
    float m_Height;

    float m_Dist;
    float m_Alpha;
    float m_filter_size;
    float m_amb_strength;
    float m_LOD;
    float m_SM_Quality;
    float m_Cartoon_sha;
    bool m_toggle_Point;
    bool m_toggle_Mirror;
    int m_toggle_ShaAmbCos;
    int m_cur_tex;
    QVector3D m_MousePos;
    //dark image
    GLuint m_Dark;
    //bright image
    GLuint m_Bright;
    //shape map
    GLuint m_ShapeMap;
    //background image
    GLuint m_BG;
    //label-depth image
    GLuint m_LD;
    //label-depth image
    GLuint m_env;

    SHADER_TYPE m_type;

private:
    QGLShader *m_VertexShader;
    QGLShader *m_FragmentShader;
    QString m_VShaderFile;
    QString m_FShaderFile;
    GLuint m_resTexture;


    GLuint m_ShaderDark;
    GLuint m_ShaderBright;
    GLuint m_ShaderShapeMap;
    GLuint m_ShaderLabel;
    GLuint m_ShaderDepth;

    //make sure glCopyTextue is called before subTex is called
    bool m_SMInitialized;
    bool m_DarkInitialized;
    bool m_BrightInitialized;
    bool m_LDInitialized;
    bool m_isInitialized;
};

#endif // SHADERPROGRAM_H
