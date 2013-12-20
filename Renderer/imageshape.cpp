#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

#include "imageshape.h"


ImageShapeCustomDialog::ImageShapeCustomDialog(ImageShape* imgS, QString title, QWidget *parent, char* execLabel, void (*callback)(), bool * ischeck)
    : CustomDialog(title, parent, execLabel,callback,ischeck)
{
    m_imgShape = imgS;
    m_imgShape->SetPenal(this);
    Initialize();

}

void ImageShapeCustomDialog::Initialize()
{
    m_returnWidth = this->addDblSpinBoxF("Width:", 0, 2, &m_imgShape->m_width, 2);
    m_returnHeight = this->addDblSpinBoxF("Height:", 0,2, &m_imgShape->m_height, 2);
    this->addDblSpinBoxF("Alpha:", 0, 1, &m_imgShape->m_alpha_th, 2);
    this->addDblSpinBoxF("Strech:", 0, 10, &m_imgShape->m_stretch, 1);
    this->addComboBox("Cur Texture", "ShapeMape|Dark|Bright|Label", &m_imgShape->m_curTexture);
    QPushButton *texButton = new QPushButton("Set Texture");
    layoutNextElement->addWidget(texButton);
    connect(texButton,SIGNAL(clicked()),this,SLOT(LoadTextureImage()));
}

void ImageShapeCustomDialog::SetNewSize(double w, double h)
{
    m_returnWidth->blockSignals(true);
    m_returnHeight->blockSignals(true);
    m_returnWidth->setValue(w);
    m_returnHeight->setValue(h);
    m_returnWidth->blockSignals(false);
    m_returnHeight->blockSignals(false);
}

void ImageShapeCustomDialog::LoadTextureImage(int cur)
{
    m_imgShape->m_curTexture = cur;
    LoadTextureImage();
}

void ImageShapeCustomDialog::LoadTextureImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        QPushButton::tr("Open Image"), "/home/", QPushButton::tr("Image Files (*.png *.jpg *.bmp)"));
    switch(m_imgShape->m_curTexture)
    {
    case 0:
        m_imgShape->m_SMFile = fileName;
        m_imgShape->m_texUpdate = ImageShape::UPDATE_SM;
        break;
    case 1:
        m_imgShape->m_DarkFile = fileName;
        m_imgShape->m_texUpdate = ImageShape::UPDATE_DARK;
        break;
    case 2:
        m_imgShape->m_BrightFile = fileName;
        m_imgShape->m_texUpdate = ImageShape::UPDATE_BRIGHT;
        break;
    }
}

ImageShape::ImageShape(int w, int h)
{
    m_width = w;
    m_height = h;
    m_texUpdate = UPDATE_SM|UPDATE_DARK|UPDATE_BRIGHT;
    m_curTexture = 0;
    m_alpha_th = 0.1;
    m_stretch = 1;
    m_penal = NULL;
}

ImageShape::~ImageShape()
{
    glDeleteTextures(1,&m_texDark);
    glDeleteTextures(1,&m_texBright);
    glDeleteTextures(1,&m_texSM);
}

void ImageShape::calAverageNormal()
{
    if(!m_SMimg.isNull())
    {
        _shaderParam.m_averageNormal = QVector3D(0.0,0.0,0.0);
        int m = 0;
        for(int i=0;i<m_SMimg.width();i++)
            for(int j=0;j<m_SMimg.height();j++)
            {
                QRgb a_color = m_SMimg.pixel(i,j);
                if(qAlpha(a_color)>m_alpha_th)
                {
                    float r = (qRed(a_color)-127)*2;
                    float g = (qGreen(a_color)-127)*2;
                    float z = 1-r*r-g*g;
                    if(z<0)
                        z = 0.01;
                    else
                        z = sqrt(z);
                     _shaderParam.m_averageNormal += QVector3D(r,g,z);
                    m++;
                }
            }
         _shaderParam.m_averageNormal /= 255*m;
    }
    qDebug()<<"normal"<< _shaderParam.m_averageNormal;
}
