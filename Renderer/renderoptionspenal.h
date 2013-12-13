#ifndef RENDEROPTIONSPENAL_H
#define RENDEROPTIONSPENAL_H

#include <QWidget>
#include <QDir>
#include <QListWidget>
#include <QPushButton>
#include <QPixmap>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>

#include "qsliderspinbox.h"
#include "../glwidget.h"
#include "shaderprogram.h"

class RenderOptionsPenal : public QWidget
{
    Q_OBJECT

public:
    RenderOptionsPenal(QWidget *parent = 0, GLWidget *program = NULL);
    void SetGL(GLWidget *p){m_RenderWindow = p;}
    QPushButton *m_ButtonSetFolder;
    QPushButton *m_ButtonSetBG;
    QPushButton *m_ButtonSetLense;
    QPushButton *m_ButtonSetEnv;
    QPushButton *m_ButtonSetDIDark;
    QPushButton *m_ButtonSetDIBright;
    QPushButton *m_ButtonSetFG;


    QPushButton *m_ButtonRefFolder;
    QPushButton *m_ButtonLoadProj;
    QPushButton *m_ButtonSaveProj;
    QPushButton *m_ButtonSave;
    QPushButton *m_ReloadShader;
    QPushButton *m_ButtonAbout;


    QSliderSpinBox *m_DepthSliderSpinbox;
    QLabel *m_DepthValue;
    QSliderSpinBox *m_AlphaSliderSpinbox;
    QLabel *m_AlphaValue;
    QSliderSpinBox *m_FilterSliderSpinbox;
    QLabel *m_FilterValue;
    QSliderSpinBox *m_AmbSliderSpinbox;
    QLabel *m_AmbValue;
    QSliderSpinBox *m_LODSliderSpinbox;
    QLabel *m_LODValue;
    QSliderSpinBox *m_SMQualitySpinbox;
    QLabel *m_SMQualityValue;
    QSliderSpinBox *m_CartoonShaSpinbox;
    QLabel *m_CartoonShaValue;

    QCheckBox *m_ShowLight;
    QComboBox *m_ShowTex;

    QCheckBox *m_ShowCos;
    QCheckBox *m_ShowAmb;
    QCheckBox *m_ShowSha;
    QCheckBox *m_ShowPoint;
    QCheckBox *m_ShowMirror;


signals:
    void SetBG(QPixmap);
    void SetLense(QPixmap);
    void SetEnv(QPixmap);
    void SetDIDark(QPixmap);
    void SetDIBright(QPixmap);
    void SetFG(QPixmap);

public slots:
    void SetFolder();
    void BuildFileList();
    void GetBG();
//    void GetLense();
//    void GetGradLense();
//    void GetDIDark();
//    void GetDIBright();
//    void GetFG();
//    void GetEnv();

    void LoadProject();
    void SaveProject();

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
    void SetCurTex(int ind);

    void SetDisplayImage(int);
    void SaveImage();
    void About();
private:
    QDir *m_FileDir;
    QStringList *m_FileNames;
    QListWidget  *m_FileWidget;
    QPixmap GetPixmap(int index,  QFileInfo &image);
    void CopyPasteFile(QString NewDir, QFileInfo FileName);

    QLabel *m_MiniMap;
    GLWidget *m_RenderWindow;
//    QPixmap applySobelFiltering(QPixmap pixmap);

    bool m_ShaderSaveCheck;
    bool m_ImageSaveCheck;

    QFileInfo m_BGFileName;
    QFileInfo m_SMFileName;
    QFileInfo m_DIDarkFileName;
    QFileInfo m_DIBrightFileName;
    QFileInfo m_ENVFileName;
    QFileInfo m_SIFileName;

    QFileInfo m_FShaderName;
    QFileInfo m_VShaderName;

protected:
    void closeEvent( QCloseEvent * event );
};

#endif // RENDEROPTIONSPENAL_H
