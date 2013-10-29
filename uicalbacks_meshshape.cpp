#include "mainwindow.h"
#include "glwidget.h"
#include "canvas.h"
#include "MeshShape/meshshape.h"
#ifdef FACIAL_SHAPE
#include "FacialShape/facialshape.h"
#endif


void MainWindow::selectExtrudeEdge()
{
    MeshShape::setOPMODE(MeshShape::EXTRUDE_EDGE);
    unselectDrag();
}

void MainWindow::selectExtrudeFace()
{
    MeshShape::setOPMODE(MeshShape::EXTRUDE_FACE);
    unselectDrag();
}

void MainWindow::selectDeleteFace()
{
    MeshShape::setOPMODE(MeshShape::DELETE_FACE);
    unselectDrag();
}

void MainWindow::selectInsertSegment()
{
    MeshShape::setOPMODE(MeshShape::INSERT_SEGMENT);
    unselectDrag();
}

void MainWindow::new2NGon()
{
    setOptionsWidget("2NGON");
    MeshShape* pMS = MeshShape::insertNGon(Point(0,0), 4,2, 0.1);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

void MainWindow::newGrid()
{
    setOptionsWidget("GRID");
    MeshShape* pMS = MeshShape::insertGrid(Point(0,0), 0.1, 2, 2);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

void MainWindow::newSpine()
{
    //MeshShape* pM = MeshShape::newMeshShape(Point(0,0),MeshShape::SPINE);
    //Canvas::get()->insert(pM);
#ifdef FACIAL_SHAPE
    MeshShape* pM = new FacialShape();
    Canvas::get()->insert(pM);
#endif
    glWidget->updateGL();
}


void MainWindow::newTorus()
{
    setOptionsWidget("TORUS");
    MeshShape* pMS = MeshShape::insertTorus(Point(0,0), 12, 0.3);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

QWidget* createNgonOptions()
{

    QWidget * ngonWidget = new QWidget;
    int rowNumber = 0;

    // slider
    QSlider *qs = new QSlider(Qt::Horizontal,ngonWidget);
    qs->setRange(1,20);
    qs->setValue(10);

    // label1
    QLabel *enterDet = new QLabel(QWidget::tr("Enter options below:"), ngonWidget);
    QLabel *label = new QLabel(QWidget::tr("Enter value of N:"), ngonWidget);
//    QLabel *label1 = new QLabel(tr("Enter value 2:"), ngonWidget);
    QPushButton *createNgonButton = new QPushButton(QWidget::tr("Create Ngon"),ngonWidget);

    // spin box
    QDoubleSpinBox       *spnBox = new QDoubleSpinBox(ngonWidget);
    spnBox->setValue(30);

//    QSpinBox       *spnBox1 = new QSpinBox(ngonWidget);
//    spnBox1->setValue(30);

    QGridLayout * layNgon = new QGridLayout;

    layNgon->addWidget(enterDet,rowNumber++,0);
    rowNumber++;

    layNgon->addWidget(label,rowNumber,0);
    layNgon->addWidget(spnBox,rowNumber++,1);
//    layNgon->addWidget(label1,rowNumber,0);
//    layNgon->addWidget(spnBox1,rowNumber++,1);
    layNgon->addWidget(new QLabel(QWidget::tr("Scale value: ")),rowNumber++,0);
    layNgon->addWidget(qs,rowNumber++,0,1,2);
    layNgon->addWidget(createNgonButton ,rowNumber++,0,1,2);
    //    layNgon->addWidget(pageComboBox,4,0);
    layNgon->setRowStretch(rowNumber,1);

    ngonWidget->setLayout(layNgon);
    return ngonWidget;
}

QWidget* createGridOptions()
{
    int rowNumber = 0;

    QWidget * gridWidget = new QWidget;
    // slider;
    QSlider *qs = new QSlider(Qt::Horizontal,gridWidget);
    qs->setRange(1,20);
    qs->setValue(10);

    // label1
    QLabel *enterDet = new QLabel(QWidget::tr("Enter options below:"), gridWidget);
    QLabel *label = new QLabel(QWidget::tr("Number of rows:"), gridWidget);
    QLabel *label1 = new QLabel(QWidget::tr("Number of columns:"), gridWidget);

    // spin box
    QDoubleSpinBox       *spnBox = new QDoubleSpinBox(gridWidget);
    spnBox->setValue(30);

    QSpinBox       *spnBox1 = new QSpinBox(gridWidget);
    spnBox1->setValue(30);

    QPushButton *createGridButton = new QPushButton(QWidget::tr("Create Grid"),gridWidget);

    QGridLayout* layGrid = new QGridLayout;

    layGrid->addWidget(enterDet,rowNumber++,0);
    rowNumber++;

    layGrid->addWidget(label,rowNumber,0);
    layGrid->addWidget(spnBox,rowNumber++,1);
    layGrid->addWidget(label1,rowNumber,0);
    layGrid->addWidget(spnBox1,rowNumber++,1);
    layGrid->addWidget(new QLabel(QWidget::tr("Scale value: ")),rowNumber++,0);
    layGrid->addWidget(qs,rowNumber++,0,1,2);
    layGrid->addWidget(createGridButton ,rowNumber++,0,1,2);
    //    layGrid->addWidget(pageComboBox,4,0);

    layGrid->setRowStretch(rowNumber,1);

    gridWidget->setLayout(layGrid);

    return gridWidget;
}


void MainWindow::createAllOptionsWidgets()
{
    addOptionsWidget(createGridOptions(),"GRID");
    addOptionsWidget(createNgonOptions(),"2NGON");
}

