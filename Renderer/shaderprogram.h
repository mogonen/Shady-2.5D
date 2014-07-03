#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "framebufferwrapper.h"
#include <QGLFramebufferObject>

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QList>
#include "shaderparameters.h"
#include "shape.h"

class shape;
class ShaderProgram : public QGLShaderProgram, public QGLFunctions
{
public:
    enum SHADER_TYPE {TYPE_RENDER, TYPE_MODEL, TYPE_COMPOSITE};
    ShaderProgram(SHADER_TYPE type = TYPE_RENDER);
    ShaderProgram(const QString &VFile, const QString &FFile);
    ~ShaderProgram();

    void GrabResultsTes();
    void Initialize();
    void InitializeParameters();
    void SetParametersToShader();

    void BindAllFBO();
    void ReleaseAllFBO();

    void LoadShader(const QString& vshader = QString(),const QString& fshader = QString());
    void ReloadShader();
    static GLuint SetTexture(QPixmap TextureMap, bool isMipmap, bool isReversed);

    void initialiFBO(int w, int h);

    void SetTextureToShader();
//    void ResetDarkImage();
//    void ResetBrightImage();
//    void ResetLDImage();
//    void ResetShadeImage();
//    void LoadDarkImage(unsigned char *data, int width, int height);
//    void LoadBrightImage(unsigned char *data, int width, int height);
    void LoadBGImage(unsigned char *data, int width, int height);
    void LoadEnvImage(unsigned char *data, int width, int height);
//    void ComputeAmbImage();

//    void InitializeTextures();
//    void GrabShapeMap(int w, int h);
//    void GrabDarkMap();
//    void GrabBrightMap();
//    void GrabLDMap();
//    void GrabShadeMap();
//    void SetShadeMapFBO();

    void SetDepthValue(double dep);
    void SetAlphaValue(double alp);
    void SetAmbValue(double amb);
    void SetFilterValue(double filter);
    void SetLevelOfDetail(double LOD);
    void SetSMQuality(double Quality);
    void SetCartoonSha(double Strength);
    void SetSurfDisp(double Disp);
//    void ToggleInfor(bool info){m_ShowInfo = info;updateGL();}
    void ToggleCos(bool info);
    void ToggleAmb(bool info);
    void ToggleSha(bool info);
    void ToggleMirror(bool info);
    void TogglePoint(bool info);
    void SetCurTex(int index);
    void SetLightPos(QVector3D light_pos);

    void ToggleNormal(bool info);
    void ToggleCenter(bool info);

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
    bool isFBOInitialized(){return m_isFBOInitialized;}
    bool isSMInitialized(){return m_isSMInitialized;}
    void SetInitialized(bool v){m_isInitialized = v;}
    void SetFBOInitialized(bool v){m_isFBOInitialized = v;}
    bool SetSMInitialized(bool v){m_isSMInitialized=v;}

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
    float m_surface_disp;
    bool m_toggle_Point;
    bool m_toggle_Mirror;
    int m_toggle_ShaAmbCos;
    int m_cur_tex;
    QVector3D m_MousePos;

    //the reaons why I am having FrameBufferWrapper is QGLFramebufferObject has bug
    // that does not work with ATI video card if there is a depth buffer.
    // added by Youyou Wang, March 17, 2014

    //dark image
    FrameBufferWrapper *m_DarkFBO;
    //bright image
    FrameBufferWrapper *m_BrightFBO;
    //shape map
    FrameBufferWrapper *m_ShapeMapFBO;
    //background image
    GLuint m_BG;
    //label-depth image
    FrameBufferWrapper *m_LDFBO;
    //label-depth image
    GLuint m_Env;
    //displacement image
    FrameBufferWrapper *m_DispFBO;
    //the results from shading/ambient/shadow
    // however, FrameBufferWrapper does not work with shader, especially the m_ShadeFBO
    // which I do not know why, but for time being, I set it as QGLFramebufferObject, scine
    // it does not use depth buffer, so it should be fine with ATI video card
    // so I have to set it as QGLFramebufferObject,
    // added by Youyou Wang, March 17, 2014
    QGLFramebufferObject *m_ShadeFBO;


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
//    bool m_SMInitialized;
//    bool m_DarkInitialized;
//    bool m_BrightInitialized;
//    bool m_LDInitialized;
//    bool m_SDInitialized;

    bool m_isInitialized;
    bool m_isFBOInitialized;
    bool m_isSMInitialized;
};

#endif // SHADERPROGRAM_H
