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
    toolsWidgetDock->setVisible(true);
	stackedWidget->setCurrentIndex(NGON);
    MeshShape* pMS = MeshShape::insertNGon(Point(0,0), 2, 0.1);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

void MainWindow::newGrid()
{
    toolsWidgetDock->setVisible(true);
    stackedWidget->setCurrentIndex(GRID);
    MeshShape* pMS = MeshShape::insertGrid(Point(0,0), 0.1, 2, 2);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

void MainWindow::newSpine()
{
    toolsWidgetDock->setVisible(true);
    stackedWidget->setCurrentIndex(SPINE);
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
    //createCustomDialog("Create NGon", "input1","input2","input3");
    MeshShape* pMS = MeshShape::insertTorus(Point(0,0), 12, 0.3);
    Canvas::get()->insert(pMS);
    glWidget->updateGL();
}

void MainWindow::createAllOptionsWidgets()
{
    stackedWidget = new QStackedWidget(toolsWidgetDock);
    layNgon = new QGridLayout;
    layGrid = new QGridLayout;
    laySpine = new QGridLayout;
    gridWidget = new QWidget;
    ngonWidget = new QWidget;
    spineWidget = new QWidget;
    createNgonOptions();
    createGridOptions();
    createSpineOptions();

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(stackedWidget);

    toolsWidgetDock->setWidget(stackedWidget);
    toolsWidgetDock->setLayout(layout);
    toolsWidgetDock->setVisible(false);
}

void MainWindow::createNgonOptions()
{
    int rowNumber = 0;

    // slider
    QSlider *qs = new QSlider(Qt::Horizontal,ngonWidget);
    qs->setRange(1,20);
    qs->setValue(10);

    // label1
    QLabel *enterDet = new QLabel(tr("Enter options below:"), ngonWidget);
    QLabel *label = new QLabel(tr("Enter value of N:"), ngonWidget);
//    QLabel *label1 = new QLabel(tr("Enter value 2:"), ngonWidget);
    QPushButton *createNgonButton = new QPushButton(tr("Create Ngon"),ngonWidget);

    // spin box
    QDoubleSpinBox       *spnBox = new QDoubleSpinBox(ngonWidget);
    spnBox->setValue(30);

//    QSpinBox       *spnBox1 = new QSpinBox(ngonWidget);
//    spnBox1->setValue(30);

    layNgon->addWidget(enterDet,rowNumber++,0);
    rowNumber++;

    layNgon->addWidget(label,rowNumber,0);
    layNgon->addWidget(spnBox,rowNumber++,1);
//    layNgon->addWidget(label1,rowNumber,0);
//    layNgon->addWidget(spnBox1,rowNumber++,1);
    layNgon->addWidget(new QLabel(tr("Scale value: ")),rowNumber++,0);
    layNgon->addWidget(qs,rowNumber++,0,1,2);
    layNgon->addWidget(createNgonButton ,rowNumber++,0,1,2);
    //    layNgon->addWidget(pageComboBox,4,0);
    layNgon->setRowStretch(rowNumber,1);

    ngonWidget->setLayout(layNgon);

    stackedWidget->addWidget(ngonWidget);


}

void MainWindow::createGridOptions()
{
    int rowNumber = 0;

    // slider
    QSlider *qs = new QSlider(Qt::Horizontal,gridWidget);
    qs->setRange(1,20);
    qs->setValue(10);

    // label1
    QLabel *enterDet = new QLabel(tr("Enter options below:"), gridWidget);
    QLabel *label = new QLabel(tr("Number of rows:"), gridWidget);
    QLabel *label1 = new QLabel(tr("Number of columns:"), gridWidget);

    // spin box
    QDoubleSpinBox       *spnBox = new QDoubleSpinBox(gridWidget);
    spnBox->setValue(30);

    QSpinBox       *spnBox1 = new QSpinBox(gridWidget);
    spnBox1->setValue(30);

    QPushButton *createGridButton = new QPushButton(tr("Create Grid"),gridWidget);

    layGrid->addWidget(enterDet,rowNumber++,0);
    rowNumber++;

    layGrid->addWidget(label,rowNumber,0);
    layGrid->addWidget(spnBox,rowNumber++,1);
    layGrid->addWidget(label1,rowNumber,0);
    layGrid->addWidget(spnBox1,rowNumber++,1);
    layGrid->addWidget(new QLabel(tr("Scale value: ")),rowNumber++,0);
    layGrid->addWidget(qs,rowNumber++,0,1,2);
    layGrid->addWidget(createGridButton ,rowNumber++,0,1,2);
    //    layGrid->addWidget(pageComboBox,4,0);

    layGrid->setRowStretch(rowNumber,1);

    gridWidget->setLayout(layGrid);

    stackedWidget->addWidget(gridWidget);

}

void MainWindow::createSpineOptions()
{
    int rowNumber = 0;

    // slider
    QSlider *qs = new QSlider(Qt::Horizontal,spineWidget);
    qs->setRange(1,20);
    qs->setValue(10);

    // label1
    QLabel *enterDet = new QLabel(tr("Enter options below:"), spineWidget);
    QLabel *label = new QLabel(tr("Number of points:"), spineWidget);
    QLabel *label1 = new QLabel(tr("Other option:"), spineWidget);

    // spin box
    QDoubleSpinBox       *spnBox = new QDoubleSpinBox(spineWidget);
    spnBox->setValue(30);

    QSpinBox       *spnBox1 = new QSpinBox(spineWidget);
    spnBox1->setValue(30);

    QPushButton *createGridButton = new QPushButton(tr("Create Spine"),spineWidget);

    laySpine->addWidget(enterDet,rowNumber++,0);
    rowNumber++;

    laySpine->addWidget(label,rowNumber,0);
    laySpine->addWidget(spnBox,rowNumber++,1);
    laySpine->addWidget(label1,rowNumber,0);
    laySpine->addWidget(spnBox1,rowNumber++,1);
    laySpine->addWidget(new QLabel(tr("Scale value: ")),rowNumber++,0);
    laySpine->addWidget(qs,rowNumber++,0,1,2);
    laySpine->addWidget(createGridButton ,rowNumber++,0,1,2);
    //    laySpine->addWidget(pageComboBox,4,0);

    laySpine->setRowStretch(rowNumber,1);

    spineWidget->setLayout(laySpine);

    stackedWidget->addWidget(spineWidget);
}
