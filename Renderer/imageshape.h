#ifndef IMAGESHAPE_H
#define IMAGESHAPE_H

#include <QString>
#include <qgl.h>

#include "./Shape.h"
#include "./PreviewAttrDialog.h"

class ImageShape;
class ImageShapeCustomDialog : public PreviewAttrDialog
{
    Q_OBJECT
public:
    ImageShapeCustomDialog(ImageShape* imgS, QString title, QWidget *parent = 0, char* execLabel = 0, void (*callback)()=0, bool * ischeck = 0);

    void                        Initialize();
    void                        SetNewSize(double w, double h);
private slots:
    void                        LoadTextureImage(int );
    void                        LoadTextureImage();

private:
    ImageShape*                 m_imgShape;
    QDoubleSpinBox*             m_returnWidth;
    QDoubleSpinBox*             m_returnHeight;
};

class ImageShape : public Shape
{
public:
    ImageShape(float w=1.0, float h=1.0);
    ~ImageShape();

    void                        render(int mode);
    //void calAverageNormal();

    void                        InitializeTex();
    void                        LoadTextureImage(int );
    void                        LoadTextureImage();
    void                        SetPenal(ImageShapeCustomDialog *penal);
    void                        renderBBox();

    void                        getBBox(BBox& bbox) const;
    void                        onApplyT(const Matrix3x3& tM);

    ShapeType                   type() const {return IMAGE_SHAPE;}
    ImageShapeCustomDialog*     GetPenal(){return m_penal;}

    enum{NO_UPDATE = 0, UPDATE_SM = 1, UPDATE_DARK = 2, UPDATE_BRIGHT = 4, UPDATE_DISP = 8} texType;

    double                      m_width;
    double                      m_height;
    int                         m_texUpdate;

    GLuint                      m_texSM;
    GLuint                      m_texDark;
    GLuint                      m_texBright;
    GLuint                      m_texDisp;

    QString                     m_fileName[ACTIVE_CHANNELS];

    /*
    QString                     m_SMFile;
    QString                     m_DarkFile;
    QString                     m_BrightFile;
    QString                     m_DispFile;
    */
    //shape map image will be saved for further use

    QImage                      m_SMimg;
    int                         m_curTexture;
    ImageShapeCustomDialog*     m_penal;
};

inline float CapValue(float x, float lo, float hi){
    return (x < lo) ? lo : ((x>hi)?hi:x);
}

#endif // IMAGESHAPE_H
