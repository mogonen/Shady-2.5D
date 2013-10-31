#include "mainwindow.h"
#include "glwidget.h"
#include "canvas.h"
#include "MeshShape/meshshape.h"
#ifdef FACIAL_SHAPE
#include "FacialShape/facialshape.h"
#endif

void executeMeshShapeOperations(){
    SelectionSet selects = Selectable::getSelection();
    FOR_ALL_ITEMS(SelectionSet, selects){
        MeshShape::execOP(Point(),*it);
    }
    MainWindow::updateGL();
}

void createGrid(){
    MeshShape* pMS =MeshShape::insertGrid(Point(), MeshShape::GRID_LEN, MeshShape::GRID_M, MeshShape::GRID_N);
    Canvas::get()->insert(pMS);
    MainWindow::updateGL();
}

void createNGon(){
    MeshShape* pMS = MeshShape::insertNGon(Point(), MeshShape::NGON_N, MeshShape::NGON_SEG_V, MeshShape::NGON_RAD);
    Canvas::get()->insert(pMS);
    MainWindow::updateGL();
}

void createTorus(){
    MeshShape* pMS = MeshShape::insertTorus(Point(), MeshShape::TORUS_N, MeshShape::TORUS_RAD);
    Canvas::get()->insert(pMS);
    MainWindow::updateGL();
}

void MainWindow::selectExtrudeEdge()
{
    setOptionsWidget(Options::EXTRUDE_E);
    MeshShape::setOPMODE(MeshShape::EXTRUDE_EDGE);
    unselectDrag();
}

void MainWindow::selectExtrudeFace()
{
    setOptionsWidget(Options::EXTRUDE_E);
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
    setOptionsWidget(Options::NGON);
}

void MainWindow::newGrid()
{
    setOptionsWidget(Options::GRID);
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
    setOptionsWidget(Options::TORUS);
}

QWidget* createNgonOptions()
{
    CustomDialog * widget = new CustomDialog("2NGon Options", 0, "Insert", createNGon);
    widget->addSpinBox("Sides:", 1, 8, &MeshShape::NGON_N, 1, "Sides of 2NGon");
    widget->addSpinBox("Segments:", 1, 4, &MeshShape::NGON_SEG_V, 1, "Rows");
    widget->addDblSpinBoxF("Radius:", 0.01, 0.75, &MeshShape::NGON_RAD, 2, 0.01, "");
    return widget;
}

QWidget* createGridOptions()
{
    CustomDialog * widget = new CustomDialog("Grid Options",0, "Insert", createGrid);
    widget->addSpinBox("Rows:", 1, 8, &MeshShape::GRID_N, 1, "Rows");
    widget->addSpinBox("Coloumns:", 1, 8, &MeshShape::GRID_M, 1, "Coloumns");
    widget->addDblSpinBoxF("Edge Length:", 0.01, 0.5, &MeshShape::GRID_LEN, 2, 0.01, "");
    return widget;
}

QWidget* createTorusOptions()
{
    CustomDialog * widget = new CustomDialog("Torus Options",0, "Insert", createTorus);
    widget->addSpinBox("Sides:", 1, 8, &MeshShape::TORUS_N, 1, "Sides of 2NGon");
    widget->addDblSpinBoxF("Radius:", 0.01, 0.75, &MeshShape::TORUS_RAD, 2, 0.01, "");
    return widget;
}


QWidget* createExtrudeOptions()
{
    CustomDialog * widget = new CustomDialog("Extrusion Options",0, "Exture", executeMeshShapeOperations, &MeshShape::EXEC_ONCLICK);

    bool * val = new bool(true);
    widget->addDblSpinBoxF("Extrusion Amount:", 0, 0.5, &MeshShape::EXTRUDE_T, 2, 0.01, "");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    // We want our custom dialog called "Registration".
    /*d.addLineEdit (input1+"  ", &Value0, "No middle name!");             // Here's a line edit.
    d.addCheckBox (input2+"  ", &Value1, "my tooltip");       // Here's a checkbox with a tooltip (optional last argument).
    d.addSpinBox  (input3+"  ", 1, 120, &Value2, 1);                   // Here's a number spin box for age.
    d.addComboBox ("Value: ", "Value1|Value2|Value3", &Value3);   // And here's a combo box with three options (separated by '|').

    // We want our custom dialog called "Registration".
    _options->addLabel    ("Please enter the details below ...");           // The first element is just a text label (non interactive).
    _options->addLineEdit (input1+"  ", &Value0, "No middle name!");             // Here's a line edit.
    _options->addCheckBox (input2+"  ", &Value1, "my tooltip");       // Here's a checkbox with a tooltip (optional last argument).
    _options->addSpinBox  (input3+"  ", 1, 120, &Value2, 1);                   // Here's a number spin box for age.
    _options->addComboBox ("Value: ", "Value1|Value2|Value3", &Value3);   // And here's a combo box with three options (separated by '|').
    */
    return widget;
}

void MainWindow::createAllOptionsWidgets()
{
    addOptionsWidget(createGridOptions(), Options::GRID);
    addOptionsWidget(createNgonOptions(), Options::NGON);
    addOptionsWidget(createTorusOptions(), Options::TORUS);
    addOptionsWidget(createExtrudeOptions(), Options::EXTRUDE_E);
}

