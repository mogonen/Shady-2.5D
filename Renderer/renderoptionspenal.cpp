#include "renderoptionspenal.h"

#include <QGridLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QtCore>
//#include <QSpinBox>

RenderOptionsPenal::RenderOptionsPenal(QWidget *parent, GLWidget *program) :
    QWidget(parent)
{

    m_RenderWindow = program;

    setWindowTitle("Shape m_ap Control Panel");

    //set slider
    m_FileWidget = new QListWidget();
    connect(m_FileWidget, SIGNAL(currentRowChanged(int)), this, SLOT(SetDisplayImage(int)));

    //set m_ini m_ap labe
     m_MiniMap = new QLabel();


    QGridLayout *mainLayout = new QGridLayout();
//    //set all buttons
//    //image related
    m_ButtonSetBG = new QPushButton("Set BG Image");
    connect(m_ButtonSetBG, SIGNAL(clicked()), this, SLOT(GetBG()));
//    m_ButtonSetLense = new QPushButton("Set Shape m_ap");
//    connect(m_ButtonSetLense, SIGNAL(clicked()), this, SLOT(GetLense()));
    m_ButtonSetEnv = new QPushButton("Set Enviroment");
    connect(m_ButtonSetEnv, SIGNAL(clicked()), this, SLOT(GetEnv()));
//    m_ButtonSetDIDark = new QPushButton("Set Diffusion(Dark)");
//    connect(m_ButtonSetDIDark, SIGNAL(clicked()), this, SLOT(GetDIDark()));
//    m_ButtonSetDIBright = new QPushButton("Set Diffusion(Bright)");
//    connect(m_ButtonSetDIBright, SIGNAL(clicked()), this, SLOT(GetDIBright()));
//    m_ButtonSetFG = new QPushButton("Set Specular Image");
//    connect(m_ButtonSetFG, SIGNAL(clicked()), this, SLOT(GetFG()));
//    m_ButtonSetFolder = new QPushButton("Set Image Folder");
//    connect(m_ButtonSetFolder, SIGNAL(clicked()), this, SLOT(SetFolder()));

//    //project related
//    m_ButtonRefFolder = new QPushButton("   Refresh Folder   ");
//    connect(m_ButtonRefFolder, SIGNAL(clicked()), this, SLOT(BuildFileList()));
//    m_ButtonLoadProj = new QPushButton("Load Project");
//    connect(m_ButtonLoadProj, SIGNAL(clicked()), this, SLOT(LoadProject()));
//    m_ButtonSaveProj = new QPushButton("Save Project");
//    connect(m_ButtonSaveProj, SIGNAL(clicked()), this, SLOT(SaveProject()));
    m_ButtonSave = new QPushButton("Save Image");
    connect(m_ButtonSave, SIGNAL(clicked()), this, SLOT(SaveImage()));

    m_ReloadShader = new QPushButton("Reload Shader");
    connect(m_ReloadShader, SIGNAL(clicked()), this, SLOT(ReloadShader()));
//    m_ButtonAbout = new QPushButton("About");
//    connect(m_ButtonAbout, SIGNAL(clicked()), this, SLOT(About()));

    //set slider
    m_DepthSliderSpinbox = new QSliderSpinBox();
    m_DepthSliderSpinbox->SetRatio(5000);
    m_DepthSliderSpinbox->SetChangeRange(-4999, 4999);
    m_DepthSliderSpinbox->SetChangeStep(0.01);

    m_DepthValue = new QLabel("Refraction:");

    m_AlphaSliderSpinbox = new QSliderSpinBox();
    m_AlphaSliderSpinbox->SetRatio(10000);
    m_AlphaSliderSpinbox->SetChangeRange(0, 10000);
    m_AlphaSliderSpinbox->SetChangeStep(0.01);
    m_AlphaValue = new QLabel("Alpha");

    m_FilterSliderSpinbox = new QSliderSpinBox();
    m_FilterSliderSpinbox->SetRatio(1250);
    m_FilterSliderSpinbox->SetChangeRange(0, 10000);
    m_FilterSliderSpinbox->SetChangeStep(0.01);
    m_FilterValue = new QLabel("Filter Size");


    m_AmbSliderSpinbox = new QSliderSpinBox();
    m_AmbSliderSpinbox->SetRatio(10000);
    m_AmbSliderSpinbox->SetChangeRange(-50000, 50000);
    m_AmbSliderSpinbox->SetChangeStep(0.01);
    m_AmbValue = new QLabel("Shadow Size");


    m_LODSliderSpinbox = new QSliderSpinBox();
    m_LODSliderSpinbox->SetRatio(1000);
    m_LODSliderSpinbox->SetChangeRange(0, 10000);
    m_LODSliderSpinbox->SetChangeStep(0.01);
    m_LODValue = new QLabel("Translucency");

    m_SMQualitySpinbox = new QSliderSpinBox();
    m_SMQualitySpinbox->SetRatio(1000);
    m_SMQualitySpinbox->SetChangeRange(0, 1000);
    m_SMQualitySpinbox->SetChangeStep(0.01);
    m_SMQualityValue = new QLabel("SM Quality");

    m_CartoonShaSpinbox = new QSliderSpinBox();
    m_CartoonShaSpinbox->SetRatio(1000);
    m_CartoonShaSpinbox->SetChangeRange(1, 1000);
    m_CartoonShaSpinbox->SetChangeStep(0.01);
    m_CartoonShaValue = new QLabel("Cartoon");


    m_SurfDispSpinbox = new QSliderSpinBox();
    m_SurfDispSpinbox->SetRatio(1000);
    m_SurfDispSpinbox->SetChangeRange(-500, 500);
    m_SurfDispSpinbox->SetChangeStep(0.01);
    m_SurfDispValue = new QLabel("Surface");
    ///attach to widget
//    mainLayout->addWidget(m_FileWidget, 0,0,6,1);


//    mainLayout->addWidget(m_ButtonSetBG,1,1,1,2);
//    mainLayout->addWidget(m_ButtonSetLense,2,1,1,2);
//    mainLayout->addWidget(m_ButtonSetDIDark,3,1,1,2);
//    mainLayout->addWidget(m_ButtonSetDIBright,4,1,1,2);
//    mainLayout->addWidget(m_ButtonSetEnv,5,1,1,2);
////    mainLayout->addWidget(m_ButtonSetFG,6,1,1,2);


//    mainLayout->addWidget(m_ButtonSetFolder,0,1,1,5);

//    mainLayout->addWidget(m_ButtonRefFolder,1,3,1,3);
//    mainLayout->addWidget(m_ButtonLoadProj,2,3,1,3);
//    mainLayout->addWidget(m_ButtonSaveProj,3,3,1,3);

    mainLayout->addWidget(m_ButtonSave,9,3,1,3);


//    mainLayout->addWidget(m_ButtonAbout,5,3,1,3);
    mainLayout->addWidget(m_ReloadShader,8,3,1,3);
    int SliderStart_pos = 0;

    mainLayout->addWidget(m_DepthSliderSpinbox->m_Slider,SliderStart_pos,0,1,1);
    mainLayout->addWidget(m_DepthSliderSpinbox,SliderStart_pos,2,1,1);
    mainLayout->addWidget(m_DepthValue,SliderStart_pos,1,1,1);
    connect(m_DepthSliderSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetDepthValue(double)));

    mainLayout->addWidget(m_AlphaSliderSpinbox->m_Slider,SliderStart_pos+1,0,1,1);
    mainLayout->addWidget(m_AlphaSliderSpinbox,SliderStart_pos+1,2,1,1);
    mainLayout->addWidget(m_AlphaValue,SliderStart_pos+1,1,1,1);
    connect(m_AlphaSliderSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetAlphaValue(double)));

    mainLayout->addWidget(m_FilterSliderSpinbox->m_Slider,SliderStart_pos+2,0,1,1);
    mainLayout->addWidget(m_FilterSliderSpinbox,SliderStart_pos+2,2,1,1);
    mainLayout->addWidget(m_FilterValue,SliderStart_pos+2,1,1,1);
    connect(m_FilterSliderSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetFilterValue(double)));

    mainLayout->addWidget(m_AmbSliderSpinbox->m_Slider,SliderStart_pos+3,0,1,1);
    mainLayout->addWidget(m_AmbSliderSpinbox,SliderStart_pos+3,2,1,1);
    mainLayout->addWidget(m_AmbValue,SliderStart_pos+3,1,1,1);
    connect(m_AmbSliderSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetAmbValue(double)));

    mainLayout->addWidget(m_CartoonShaSpinbox->m_Slider,SliderStart_pos+4,0,1,1);
    mainLayout->addWidget(m_CartoonShaSpinbox,SliderStart_pos+4,2,1,1);
    mainLayout->addWidget(m_CartoonShaValue,SliderStart_pos+4,1,1,1);
    connect(m_CartoonShaSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetCartoonSha(double)));


    mainLayout->addWidget(m_LODSliderSpinbox->m_Slider,SliderStart_pos+5,0,1,1);
    mainLayout->addWidget(m_LODSliderSpinbox,SliderStart_pos+5,2,1,1);
    mainLayout->addWidget(m_LODValue,SliderStart_pos+5,1,1,1);
    connect(m_LODSliderSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetLevelOfDetail(double)));

    mainLayout->addWidget(m_SMQualitySpinbox->m_Slider,SliderStart_pos+6,0,1,1);
    mainLayout->addWidget(m_SMQualitySpinbox,SliderStart_pos+6,2,1,1);
    mainLayout->addWidget(m_SMQualityValue,SliderStart_pos+6,1,1,1);
    connect(m_SMQualitySpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetSMQuality(double)));

    mainLayout->addWidget(m_SurfDispSpinbox->m_Slider,SliderStart_pos+7,0,1,1);
    mainLayout->addWidget(m_SurfDispSpinbox,SliderStart_pos+7,2,1,1);
    mainLayout->addWidget(m_SurfDispValue,SliderStart_pos+7,1,1,1);
    connect(m_SurfDispSpinbox, SIGNAL(SliderSpinboxValueChange(double)), this, SLOT(SetSurfDisplacement(double)));


    //add check box
    m_ShowLight = new QCheckBox();
    mainLayout->addWidget(m_ShowLight,SliderStart_pos,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle Light"),SliderStart_pos,4,1,2);
    connect(m_ShowLight, SIGNAL(toggled(bool)), this, SLOT(ToggleInfor(bool)));

    m_ShowCos = new QCheckBox();
    m_ShowCos->setChecked(true);
    mainLayout->addWidget(m_ShowCos,SliderStart_pos+1,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle Cos"),SliderStart_pos+1,4,1,2);
    connect(m_ShowCos, SIGNAL(toggled(bool)), this, SLOT(ToggleCos(bool)));

    m_ShowAmb = new QCheckBox();
    m_ShowAmb->setChecked(true);
    mainLayout->addWidget(m_ShowAmb,SliderStart_pos+2,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle Amb"),SliderStart_pos+2,4,1,2);
    connect(m_ShowAmb, SIGNAL(toggled(bool)), this, SLOT(ToggleAmb(bool)));

    m_ShowSha = new QCheckBox();
    m_ShowSha->setChecked(true);
    mainLayout->addWidget(m_ShowSha,SliderStart_pos+3,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle Sha"),SliderStart_pos+3,4,1,2);
    connect(m_ShowSha, SIGNAL(toggled(bool)), this, SLOT(ToggleSha(bool)));

    m_ShowPoint = new QCheckBox();
    m_ShowPoint->setChecked(true);
    mainLayout->addWidget(m_ShowPoint,SliderStart_pos+4,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle Point"),SliderStart_pos+4,4,1,2);
    connect(m_ShowPoint, SIGNAL(toggled(bool)), this, SLOT(TogglePoint(bool)));


    m_ShowMirror = new QCheckBox();
    m_ShowMirror->setChecked(false);
    mainLayout->addWidget(m_ShowMirror,SliderStart_pos+5,3,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Toggle m_irror"),SliderStart_pos+5,4,1,2);
    connect(m_ShowMirror, SIGNAL(toggled(bool)), this, SLOT(ToggleMirror(bool)));


    mainLayout->addWidget(m_ButtonSetBG,SliderStart_pos+6,3,1,2);
    mainLayout->setAlignment(Qt::AlignTop);

    mainLayout->addWidget(m_ButtonSetEnv,SliderStart_pos+7,3,1,2);
    mainLayout->setAlignment(Qt::AlignTop);

    m_ShowTex = new QComboBox();
    m_ShowTex->addItems(QString("None|ShapeMap|Dark|Bright|Background|Label|Depth|Enviroment|Displacement").split("|", QString::SkipEmptyParts));
    mainLayout->addWidget(m_ShowTex,SliderStart_pos+8,0,1,1, Qt::AlignRight);
    mainLayout->addWidget(new QLabel("Current Tex"),SliderStart_pos+8,1,1,2);
    connect(m_ShowTex,SIGNAL(currentIndexChanged(int)), this, SLOT(SetCurTex(int)));

    //set m_ain layout

    setLayout(mainLayout);

//    m_RenderWindow->show();
//    connect(this,SIGNAL(SetBG(QPixmap)), m_RenderWindow, SLOT(SetBGTexture(QPixmap)));
//    connect(this,SIGNAL(SetLense(QPixmap)), m_RenderWindow, SLOT(SetLenseTexture(QPixmap)));
//    connect(this,SIGNAL(SetEnv(QPixmap)), m_RenderWindow, SLOT(SetEnvTexture(QPixmap)));
//    connect(this,SIGNAL(SetDIDark(QPixmap)), m_RenderWindow, SLOT(SetDIDarkTexture(QPixmap)));
//    connect(this,SIGNAL(SetDIBright(QPixmap)), m_RenderWindow, SLOT(SetDIBrightTexture(QPixmap)));
//    connect(this,SIGNAL(SetFG(QPixmap)), m_RenderWindow, SLOT(SetFGTexture(QPixmap)));
//    connect(this,SIGNAL(SetFG(QPixmap)), m_RenderWindow, SLOT(SetFGTexture(QPixmap)));


    //set initial value of slider controllers
    m_AlphaSliderSpinbox->setValue(1);
    m_DepthSliderSpinbox->setValue(0);
    m_FilterSliderSpinbox->setValue(2);
    m_AmbSliderSpinbox->setValue(0.25);
    m_LODSliderSpinbox->setValue(0);
    m_SMQualitySpinbox->setValue(0.5);
    m_CartoonShaSpinbox->setValue(0.1);
    m_SurfDispSpinbox->setValue(0.1);

    m_FileDir = NULL;

//    //adjust positions
//    m_ove(100,50);
//    adjustSize();
//    m_RenderWindow->setWindowTitle("Rendered Scene");
//    m_RenderWindow->move(pos().x()+size().width()+15,pos().y());
//    m_MiniMap->setWindowTitle("Image Preview");
//    m_MiniMap->show();
//    m_MiniMap->move(pos().x(),pos().y()+size().height()+40);
//    m_MiniMap->setPixmap(QPixmap(size().width(),sizeHint().width()));
//    m_MiniMap->adjustSize();
//    m_RenderWindow->sizeHint();
//    m_RenderWindow->resize(m_MiniMap->height()+size().height()+40,mMiniMap->height()+size().height()+40);
//    QSizePolicy pol;
//    pol.setHeightForWidth(true);
//    m_RenderWindow->setSizePolicy(pol);
//    m_MiniMap->setScaledContents(true);
}

void RenderOptionsPenal::SetFolder()
{
    if(m_FileDir!=NULL)
        delete m_FileDir;
    m_FileDir = new QDir(QFileDialog::getExistingDirectory(this, tr("Open Directory"), QDir::currentPath(), QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks));
    BuildFileList();
}

void RenderOptionsPenal::BuildFileList()
{
    if(m_FileDir!=NULL)
    {
        m_FileWidget->blockSignals(true);
        m_FileWidget->clear();
        QStringList nameFilter;
        nameFilter << "*.png" << "*.jpg" << "*.gif"<<"*.bmp"<<"*.jpeg"<<"*.pbm"<<"*.pgm"<<"*.ppm";
        QStringList list = m_FileDir->entryList (nameFilter);
        m_FileWidget->addItems(list);
        m_FileWidget->blockSignals(false);
    }
    else
    {
        QMessageBox::warning( this, "Empty", "No folder is selected" );
    }
}

void RenderOptionsPenal::GetBG()
{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(),m_BGFileName);
//    if(!img.isNull())
//        emit SetBG(img);
    QString fileName = QFileDialog::getOpenFileName(this,
    QPushButton::tr("Open Image"), "/home/jana", QPushButton::tr("Image Files (*.png *.jpg *.bmp)"));
    QImage LoadedImage;
    if(LoadedImage.load(fileName))
    {
        ShaderProgram *ShaderP = m_RenderWindow->getRShader();
        QImage GLImage = m_RenderWindow->convertToGLFormat(LoadedImage);
        if(ShaderP&&ShaderP->isInitialized())
        {
            ShaderP->bind();
            ShaderP->LoadBGImage(GLImage.bits(),GLImage.width(),GLImage.height());
        }
        m_RenderWindow->updateGL();
    }
}

void RenderOptionsPenal::GetEnv()
{
    QString fileName = QFileDialog::getOpenFileName(this,
    QPushButton::tr("Open Image"), "/home/jana", QPushButton::tr("Image Files (*.png *.jpg *.bmp)"));
    QImage LoadedImage;
    if(LoadedImage.load(fileName))
    {
        ShaderProgram *ShaderP = m_RenderWindow->getRShader();
        QImage GLImage = m_RenderWindow->convertToGLFormat(LoadedImage);
        if(ShaderP&&ShaderP->isInitialized())
        {
            ShaderP->bind();
            ShaderP->LoadEnvImage(GLImage.bits(),GLImage.width(),GLImage.height());
        }
        m_RenderWindow->updateGL();
    }
}

//void RenderOptionsPenal::GetLense()
//{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(),m_SMFileName);
//    if(!img.isNull())
//        emit SetLense(img);
//}

//void RenderOptionsPenal::GetGradLense()
//{
//    QPixmap img =applySobelFiltering( GetPixmap(m_FileWidget->currentRow(), m_SMFileName));
//    if(!img.isNull())
//        emit SetLense(img);
//}

//void RenderOptionsPenal::GetDIDark()
//{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(), m_DIDarkFileName);
//    if(!img.isNull())
//        emit SetDIDark(img);
//}

//void RenderOptionsPenal::GetDIBright()
//{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(), m_DIBrightFileName);
//    if(!img.isNull())
//        emit SetDIBright(img);
//}

//void RenderOptionsPenal::GetEnv()
//{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(), m_ENVFileName);
//    if(!img.isNull())
//        emit SetEnv(img);
//}



//void RenderOptionsPenal::GetFG()
//{
//    QPixmap img = GetPixmap(m_FileWidget->currentRow(), m_SIFileName);
//    if(!img.isNull())
//        emit SetFG(img);
//}


void RenderOptionsPenal::SaveProject()
{

    //Get the file path
    QString fileName = QFileDialog::getSaveFileName(this, tr("Open File"),
                                                    "/home/Desktop/myShapeMapProj.smp",
                                                    tr("Files (*.smp *.smf)"));


    //try to open file as read m_ode
    if(!fileName.isNull())
    {
        if(fileName.endsWith("smp"))
        {
            QFile file(fileName);
            //Try to open the file as write m_ode
            if(file.open( QIODevice::WriteOnly))
            {

                QTextStream ts(&file);
                ts<<"#Project grecated at "<<QTime::currentTime().toString()<<' '<<QDate::currentDate().toString()<<"\n";
                ts<<"BG="<<m_BGFileName.absoluteFilePath()<<"\n";
                ts<<"SM="<<m_SMFileName.absoluteFilePath()<<"\n";
                ts<<"DI(Dark)="<<m_DIDarkFileName.absoluteFilePath()<<"\n";
                ts<<"DI(Bright)="<<m_DIBrightFileName.absoluteFilePath()<<"\n";
                ts<<"ENV=" <<m_ENVFileName.absoluteFilePath()<<"\n";
                ts<<"Depth="<<m_DepthSliderSpinbox->GetDoubleValue()<<"\n";
                ts<<"Alpha="<<m_AlphaSliderSpinbox->GetDoubleValue()<<"\n";
                ts<<"FilterSize="<<m_FilterSliderSpinbox->GetDoubleValue()<<"\n";
                ts<<"ShadowSize="<<m_AmbSliderSpinbox->GetDoubleValue()<<"\n";
                ts<<"SMQuality="<<m_SMQualitySpinbox->GetDoubleValue()<<"\n";
                ts<<"Translucency="<<m_LODSliderSpinbox->GetDoubleValue()<<"\n";
                ts<<"Cartoon="<<m_CartoonShaSpinbox->GetDoubleValue()<<"\n";
//                qDebug()<<m_CartoonShaSpinbox->GetDoubleValue();


                ts<<"ShowLight="<<(int)m_ShowLight->isChecked()<<"\n";
                ts<<"ShowCos="<<(int)m_ShowCos->isChecked()<<"\n";
                ts<<"ShowAmb="<<(int)m_ShowAmb->isChecked()<<"\n";
                ts<<"ShowSha="<<(int)m_ShowSha->isChecked()<<"\n";
                ts<<"ShowPoint="<<(int)m_ShowPoint->isChecked()<<"\n";
                ts<<"ShowMirror="<<(int)m_ShowMirror->isChecked()<<"\n";

//                ts<<"LightPos=["<<m_RenderWindow->m_MousePos.x()<<","<<m_RenderWindow->m_MousePos.y()<<","<<m_RenderWindow->m_MousePos.z()<<"]\n";
            }
            else
                QMessageBox::critical(this, tr("Save File"),
                                      tr("could not create file"));
            file.close();
        }
        else
            if(fileName.endsWith("smf"))
            {
                QFileInfo fi(fileName);
//                qDebug()<<fi.absolutePath()+fi.baseName();
                QString newDir = fi.absolutePath()+'/'+fi.baseName();
                QDir().mkdir(newDir);
                QFile file(newDir+'/'+fi.baseName()+".smf");
                if(file.open( QIODevice::WriteOnly))
                {
                    QTextStream ts(&file);
                    ts<<"#Project grecated at "<<QTime::currentTime().toString()<<' '<<QDate::currentDate().toString()<<"\n";

                    ts<<"BG="<<m_BGFileName.fileName()<<"\n";
                    CopyPasteFile(newDir, m_BGFileName);

                    ts<<"SM="<<m_SMFileName.fileName()<<"\n";
                    CopyPasteFile(newDir, m_SMFileName);

                    ts<<"DI(Dark)="<<m_DIDarkFileName.fileName()<<"\n";
                    CopyPasteFile(newDir, m_DIDarkFileName);

                    ts<<"DI(Bright)="<<m_DIBrightFileName.fileName()<<"\n";
                    CopyPasteFile(newDir, m_DIBrightFileName);

                    ts<<"ENV=" <<m_ENVFileName.fileName()<<"\n";
                    CopyPasteFile(newDir, m_ENVFileName);

                    ts<<"Depth="<<m_DepthSliderSpinbox->GetDoubleValue()<<"\n";
                    ts<<"Alpha="<<m_AlphaSliderSpinbox->GetDoubleValue()<<"\n";
                    ts<<"FilterSize="<<m_FilterSliderSpinbox->GetDoubleValue()<<"\n";
                    ts<<"ShadowSize="<<m_AmbSliderSpinbox->GetDoubleValue()<<"\n";
                    ts<<"SMQuality="<<m_SMQualitySpinbox->GetDoubleValue()<<"\n";
                    ts<<"Translucency="<<m_LODSliderSpinbox->GetDoubleValue()<<"\n";
                    ts<<"Cartoon="<<m_CartoonShaSpinbox->GetDoubleValue()<<"\n";
//                    ts<<"LightPos=["<<m_RenderWindow->m_MousePos.x()<<","<<m_RenderWindow->m_MousePos.y()<<","<<m_RenderWindow->m_MousePos.z()<<"]\n";

                    ts<<"ShowLight="<<(int)m_ShowLight->isChecked()<<"\n";
                    ts<<"ShowCos="<<(int)m_ShowCos->isChecked()<<"\n";
                    ts<<"ShowAmb="<<(int)m_ShowAmb->isChecked()<<"\n";
                    ts<<"ShowSha="<<(int)m_ShowSha->isChecked()<<"\n";
                    ts<<"ShowPoint="<<(int)m_ShowPoint->isChecked()<<"\n";
                    ts<<"ShowMirror="<<(int)m_ShowMirror->isChecked()<<"\n";
                }
                file.close();
            }
            else
                QMessageBox::critical(this, tr("Save File"),
                                      tr("could not create file"));

    }
    else
        QMessageBox::critical(this, tr("Save File"),
                              tr("file does not exist"));
}

void RenderOptionsPenal::CopyPasteFile(QString NewDir, QFileInfo FileName)
{
    if(FileName.size())
    {
        QFile Temp(FileName.absoluteFilePath());
        if(!Temp.copy(NewDir+'/'+FileName.fileName()))
            QMessageBox::critical(this, FileName.baseName()+" File",
                                  FileName.baseName()+" can not be copied");
    }
}


void RenderOptionsPenal::LoadProject()
{
//    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
//                                                    "/home/Desktop/myShapeMapProj.smp",
//                                                    tr("Files (*.smp *.smf)"));
//    if(!fileName.isNull())
//    {
//        QFile file(fileName);
//        QImage temp_img(1,1,QImage::Format_ARGB32);
//        temp_img.setPixel(0,0,QColor(0,0,0,0).rgba());
//        QPixmap empty_img = QPixmap::fromImage(temp_img);
//        //try to open file as read m_ode
//        if(file.open( QIODevice::ReadOnly))
//        {
//            QString curPath;
//            if(fileName.endsWith("smp"))    //file is saved as text file
//                curPath = "";
//            else
//                if(fileName.endsWith("smf"))    //file is saved as text file
//                {
//                    QFileInfo projFile(fileName);
//                    curPath = projFile.absolutePath()+"/";
//                    if(m_FileDir!=NULL)
//                        delete m_FileDir;
//                    m_FileDir = new QDir(projFile.absolutePath());
//                    BuildFileList();
//                }

//            QTextStream strm(&file);
//            QString a_line;
//            //Jumpe over the '#' lines
//            do{
//                a_line=strm.readLine();
//            }
//            while(a_line[0]=='#');
//            //Read in the actaul description parts of BSpline
//            do
//            {
//                if(a_line[0]!='#')
//                {
//                    QStringList temp = a_line.split(QRegExp("(\\=|\\,|\\[|\\])"), QString::SkipEmptyParts);
//                    if(temp.at(0)==QString("BG"))
//                    {
//                        if(temp.size()>=2)
//                        {
//                            m_BGFileName = QFileInfo(curPath+temp.at(1));
//                            QPixmap img = QPixmap(curPath+temp.at(1));
//                            if(!img.isNull())
//                                emit SetBG(img);
//                            else
//                                QMessageBox::warning( this, "BG", "Stroed Back Ground Image can not be found" );
//                        }
//                        else
//                        {
//                            emit SetBG(empty_img);
//                        }
//                    }
//                    else if(temp.at(0)==QString("SM"))
//                    {
//                        if(temp.size()>=2)
//                        {
//                            m_SMFileName = QFileInfo(curPath+temp.at(1));
//                            QPixmap img = QPixmap(curPath+temp.at(1));
//                            if(!img.isNull())
//                                emit SetLense(img);
//                            else
//                                QMessageBox::warning( this, "SM", "Stroed Shape m_ap Image can not be found" );
//                        }
//                        else
//                        {
//                            emit SetLense(empty_img);
//                        }
//                    }
//                    else if(temp.at(0)==QString("DI(Dark)"))
//                    {
//                        if(temp.size()>=2)
//                        {
//                            m_DIDarkFileName = QFileInfo(curPath+temp.at(1));
//                            QPixmap img = QPixmap(curPath+temp.at(1));
//                            if(!img.isNull())
//                                emit SetDIDark(img);
//                            else
//                                QMessageBox::warning( this, "DI(Dark)", "Stroed Diffusion Image(Dark) can not be found" );
//                        }
//                        else
//                        {
//                            emit SetDIDark(empty_img);
//                        }
//                    }
//                    else if(temp.at(0)==QString("DI(Bright)"))
//                    {
//                        if(temp.size()>=2)
//                        {
//                            m_DIBrightFileName = QFileInfo(curPath+temp.at(1));
//                            QPixmap img = QPixmap(curPath+temp.at(1));
//                            if(!img.isNull())
//                                emit SetDIBright(img);
//                            else
//                                QMessageBox::warning( this, "DI(Bright)", "Stroed Diffusion Image(Bright) can not be found" );
//                        }
//                        else
//                        {
//                            emit SetDIBright(empty_img);
//                        }
//                    }
//                    else if(temp.at(0)==QString("ENV"))
//                    {
//                        if(temp.size()>=2)
//                        {
//                            m_ENVFileName = QFileInfo(curPath+temp.at(1));
//                            QPixmap img = QPixmap(curPath+temp.at(1));
//                            if(!img.isNull())
//                                emit SetEnv(img);
//                            else
//                                QMessageBox::warning( this, "Env", "Stroed Enviromental m_ap can not be found" );
//                        }
//                        else
//                        {
//                            emit SetEnv(empty_img);
//                        }
//                    }
//                    else if(temp.at(0)==QString("Depth"))
//                    {
//                        m_DepthSliderSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("Alpha"))
//                    {
//                        m_AlphaSliderSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("FilterSize"))
//                    {
//                        m_FilterSliderSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("ShadowSize"))
//                    {
//                        m_AmbSliderSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("Translucency"))
//                    {
//                        m_LODSliderSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("SMQuality"))
//                    {
//                        m_SMQualitySpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("Cartoon"))
//                    {
//                        m_CartoonShaSpinbox->SetSliderValue(temp.at(1).toDouble());
//                    }
//                    else if(temp.at(0)==QString("LightPos"))
//                    {
//                        if(temp.size()>=4)
//                        {
//                            //                            m_RenderWindow->SetMousePos(temp.at(1).toDouble(), temp.at(2).toDouble(),temp.at(3).toDouble());
//                        }
//                        else
//                            QMessageBox::warning( this, "Light", "Light Information m_ust be in [x,y,z] format" );
//                    }
//                    else if(temp.at(0)==QString("ShowLight"))
//                    {
//                        m_ShowLight->setChecked(temp.at(1).toInt());
//                    }
//                    else if(temp.at(0)==QString("ShowCos"))
//                    {
//                        m_ShowCos->setChecked(temp.at(1).toInt());
//                    }
//                    else if(temp.at(0)==QString("ShowAmb"))
//                    {
//                        m_ShowAmb->setChecked(temp.at(1).toInt());
//                    }
//                    else if(temp.at(0)==QString("ShowSha"))
//                    {
//                        m_ShowSha->setChecked(temp.at(1).toInt());
//                    }
//                    else if(temp.at(0)==QString("ShowPoint"))
//                    {
//                        m_ShowPoint->setChecked(temp.at(1).toInt());

//                    }
//                    else if(temp.at(0)==QString("ShowMirror"))
//                    {
//                        m_ShowMirror->setChecked(temp.at(1).toInt());
//                    }
//                }
//                else
//                    break;
//                a_line=strm.readLine();
//                //Stop if encounters a '#' or end of file
//            }while(!(a_line.isNull()));
//        }
//        else
//            QMessageBox::critical(this, tr("Save File"),
//                                  tr("could not create file"));
//    }
//    else
//        QMessageBox::critical(this, tr("Save File"),
//                              tr("file does not exist"));

}

void RenderOptionsPenal::SetDisplayImage(int)
{
    QFileInfo cur_name;
    QPixmap img = GetPixmap(m_FileWidget->currentRow(),cur_name);
    if(!img.isNull())
    {
        m_MiniMap->setPixmap(img);
        m_MiniMap->show();
    }
}

void RenderOptionsPenal::SaveImage()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "/home/untitled.png",
                               tr("Images (*.png *.jpg *.gif *.bmp *.jpeg *.pbm *.pgm *.ppm)"));

    if(!fileName.isNull())
    {
        QImage image = (m_RenderWindow->grabFrameBuffer());
        if( !image.save(fileName))
        {
           QMessageBox::warning( this, "Save Image", "Error saving image." );
        }
    }
}

void RenderOptionsPenal::About()
{
   QMessageBox::about(this, tr("Shape m_ap"),
               QString("<p><b>Control Penal</b></p>"
                       "<pre><b>Buttons:</b>\n"
                       "<b>Set Image Folder:</b> Set the folder for all images. All images in the set folder will show in the list.\n"
                       "<b>Set BG Image:</b> Refracted image.\n"
                       "<b>Set Shape m_ap:</b> Aspect ratio will change accordingly, and all other images will be resized to fit current shape m_ap.\n"
                       "<b>Set Diffusion(Dark):</b> The DI0 image in paper.\n"
                       "<b>Set Diffusion(Bright):</b> The DI1 image in paper.\n"
                       "<b>Set Enviroment m_ap:</b> Reflected images.\n"
                       "<b>Set Image Folder:</b> Set the folder for all images.\n"
                       "<b>Refresh Folder:</b> Refresh current image folder.\n"
                       "<b>Load Project:</b> Load a project file.\n"
                       "<b>Save Project:</b> Save a project file.\n"
                       "A project file is a text file that contains all the current parameters as well as images(names).\n"
                       "A project with extension of '.smp' will only save the absolute path/name of images.\n"
                       "A project with extension of '.smf' will save the actual image files to a folder that has the same name as project name.\n"
                       "Within project file, if an image name is left empty, then a black(rgba = 0,0,0,0) image will be set instead\n"
                       "If the given image name is not readable, a warning will be given, but no image will be set\n"
                       "<b>Save Image:</b> Save current image in the rendering window.</pre>"
                       "<pre><b>Check Boxes</b>\n"
                       "Choose to see whether a certain effect should be turnned.\n"
                       "<b>Toggle Light:</b> Show/off lighting position.\n"
                       "<b>Toggle Cos:</b> Add/remove Shading effect.\n"
                       "<b>Toggle Amb:</b> Add/remove Ambient occlusion.\n"
                       "<b>Toggle Sha:</b> Add/remove Shadow effect.\n"
                       "<b>Toggle Point:</b> Switch bettwen parallel light and point light.</pre>"
                       "<pre><b>Sliders</b>\n"
                       "Refraction:</b> Amount of distortion during refraction.\n"
                       "<b>Alpha:</b> Transparency of current shape m_ap.\n"
                       "<b>Filter Size:</b> Filter Size for calculating ambient occlusion and shadow.\n"
                       "<b>Shadow Size:</b> Threshold to decisde whether a pixel is in shadow or not.\n"
                       "<b>Cartoon:</b> Sharpness between shadow and non-shadow area.\n"
                       "<b>Transluceny:</b> Amount of transluceny for refration.\n"
                       "<b>SM Quality:</b> Flatness of Current shape m_ap, a small value will m_ake current shape m_ap flat in order to eliminate some errors.</pre>"
                       "<p><b>Rendering Window</b></p>"
                       "<pre><b>Left_Button:</b> change lighting postion.\n"
                       "<b>Right_Button:</b> change lighting intensity.\n"
                       "<b>Ctrl+Left_Button:</b> Translate image\n"
                       "<b>Ctrl+Right_Button:</b> Zoomming image\n"
                       "<b>Drag on window</b>: Resize image\n"
                       "<b>Space</b>: Restore to default size\n"
                       "<b>Tab</b>: Turn on/off light auto m_otion\n"
                       ));
}

QPixmap RenderOptionsPenal::GetPixmap(int index, QFileInfo & img_name)
{
//    if(index<0||index>=mFileWidget->count())
//    {
//        QMessageBox::critical(this, tr("Image Setting"),
//                                 tr("Cannot load selected image"));
//        return NULL;
//    }
//    img_name = QFileInfo(m_FileDir->path()+"/"+mFileWidget->currentItem()->text());
    QPixmap img(img_name.absoluteFilePath());
    return img;
}

void RenderOptionsPenal::closeEvent( QCloseEvent * event)
{
//    m_MiniMap->close();
//    m_RenderWindow->close();
}


void RenderOptionsPenal::SetDepthValue(double dep)
{
    ShaderProgram *ShaderP = m_RenderWindow->getCShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetDepthValue(dep);
    }
    m_RenderWindow->updateGL();
}
void RenderOptionsPenal::SetAlphaValue(double alp)
{
    ShaderProgram *ShaderP = m_RenderWindow->getCShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetAlphaValue(alp);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetFilterValue(double filter)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetFilterValue(filter);
    }
    m_RenderWindow->updateGL();
}
void RenderOptionsPenal::SetAmbValue(double amb)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetAmbValue(amb);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetLevelOfDetail(double LOD)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetLevelOfDetail(LOD);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetSMQuality(double Quality)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetSMQuality(Quality);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetCartoonSha(double Strength)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetCartoonSha(Strength);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetSurfDisplacement(double surf_disp)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetSurfDisp(surf_disp);
    }
    m_RenderWindow->updateGL();

}


void RenderOptionsPenal::ToggleCos(bool info)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->ToggleCos(info);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::ToggleAmb(bool info)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->ToggleAmb(info);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::ToggleSha(bool info)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->ToggleSha(info);
    }
    m_RenderWindow->updateGL();

}

void RenderOptionsPenal::ToggleMirror(bool info)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->ToggleMirror(info);
    }
    m_RenderWindow->updateGL();

}

void RenderOptionsPenal::TogglePoint(bool info)
{
    ShaderProgram *ShaderP = m_RenderWindow->getRShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->TogglePoint(info);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::SetCurTex(int index)
{
    ShaderProgram *ShaderP = m_RenderWindow->getCShader();
    if(ShaderP&&ShaderP->isInitialized())
    {
        ShaderP->bind();
        ShaderP->SetCurTex(index);
    }
    m_RenderWindow->updateGL();
}

void RenderOptionsPenal::ReloadShader()
{
    m_RenderWindow->reloadShader();
    m_RenderWindow->updateGL();
}


