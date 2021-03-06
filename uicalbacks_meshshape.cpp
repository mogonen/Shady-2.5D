#include "mainwindow.h"
#include "glwidget.h"
#include "canvas.h"
#include "MeshShape/meshshape.h"
#include "MeshShape/patch.h"
#include "MeshShape/spineshape.h"
#include "MeshShape/meshcommands.h"
#include "commands.h"

#include "Renderer/imageshape.h"
#ifdef FACIAL_SHAPE
//#include "FacialShape/facialshape.h"
#endif

void exec(){
    Session::get()->exec();
}


QWidget* createAttrWidget(Shape_p pShape)
{    
    //widget->addSpinBox("test:", 1, 8, &MeshShape::GRID_N, 1, "Rows");
    //widget->addSpinBox("U segments:", 2, 24, &Patch::N, 1);
    //widget->addColorSel("Diffuse:", &pShape->diffuse, "");
    CustomDialog * widget = new CustomDialog("Shape Attr");
    QObject::connect(widget,SIGNAL(ValueUpdated()),Session::get()->glWidget(),SLOT(updateActive()));
    return widget;
}

void executeMeshShapeOperations()
{
    //MeshShape::executeStackOperation();
    MainWindow::updateGL();
}

void createGrid()
{
    Session::get()->setCommand(new MeshPrimitive(MeshPrimitive::GRID));
    Session::get()->exec();
    //Session::get()->mainWindow()->addAttrWidget(createShapePreviewAttrWidget(Session::get()->theShape()), (void*)Session::get()->theShape());
}

void createNGon()
{
    Session::get()->setCommand(new MeshPrimitive(MeshPrimitive::TWO_NGON));
    Session::get()->exec();

}

void createTorus(){
   Session::get()->setCommand(new MeshPrimitive(MeshPrimitive::TORUS));
   Session::get()->exec();
    //Session::get()->mainWindow()->addAttrWidget(createAttrWidget((Shape_p)pMS), (void*)pMS);
}

void createSpine()
{
    Shape_p shape = Session::get()->theShape();
    SpineShape* spine = dynamic_cast<SpineShape*>(shape);
    if (spine==0)
        return;
    Shape_p pMS = spine->buildMeshShape(SpineShape::isTRIANGULATE_JOINTS);
    Session::get()->removeShape(spine);
    Session::get()->mainWindow()->addAttrWidget(createAttrWidget(pMS), (void*)pMS);
    Session::get()->insertShape(pMS);
}


void MainWindow::selectExtrudeEdge()
{
    setOptionsWidget(Options::EXTRUDE);
    Session::get()->setCommand(new MeshOperation(MeshOperation::EXTRUDE_EDGE));
    unselectDrag();
}

void MainWindow::selectExtrudeFace()
{
    setOptionsWidget(Options::EXTRUDE);
    Session::get()->setCommand(new MeshOperation(MeshOperation::EXTRUDE_FACE));
    unselectDrag();
}

void MainWindow::selectDeleteFace()
{
    setOptionsWidget(Options::NONE);
    Session::get()->setCommand(new MeshOperation(MeshOperation::DELETE_FACE));
    unselectDrag();
}

void MainWindow::selectOrderFace()
{
    setOptionsWidget(Options::ORDER_FACE);
    Session::get()->setCommand(new OrderFace());
    unselectDrag();
}

void MainWindow::selectInsertSegment()
{
    setOptionsWidget(Options::INSERT_SEGMENT);
    Session::get()->setCommand(new MeshOperation(MeshOperation::INSERT_SEGMENT));
    unselectDrag();
}

void MainWindow::selectAssignPatternTool(){
    setOptionsWidget(Options::ASSIGN_PATTERN);
    Session::get()->setCommand(new MeshOperation(MeshOperation::ASSIGN_PATTERN));
    //MeshShape::setOPMODE(MeshShape::ASSIGN_PATTERN);
    unselectDrag();
}

void MainWindow::selectSetFoldsTool(){
    setOptionsWidget(Options::SET_FOLDS);
    Session::get()->setCommand(new MeshOperation(MeshOperation::SET_FOLDS));
    unselectDrag();
}

void MainWindow::selectSetColorTool(){
    setOptionsWidget(Options::SET_COLOR);
    Session::get()->setCommand(new SetColor());
    unselectDrag();
}

void MainWindow::selectSewTool(){
    setOptionsWidget(Options::SEW);
    Session::get()->setCommand(new Sew());
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
    setOptionsWidget(Options::SPINE);
    //Session::get()->glWidget()->insertShape(new SpineShape());
    Session::get()->setCommand(new MeshPrimitive(MeshPrimitive::SPINE));
    unselectDrag();
}

void MainWindow::newTorus()
{
    setOptionsWidget(Options::TORUS);
}

void MainWindow::newFacial()
{
#ifdef FACIAL_SHAPE
    glWidget->insertShape(new FacialShape());
    //unselectDrag();
#endif
}

void MainWindow::newImageShape()
{
    setOptionsWidget(Options::IMAGE_SHAPE);
}

QWidget* createDragOptions()
{
    CustomDialog * widget = new CustomDialog("Drag Tool Options",0);
    widget->addRadioGrp("Tool:","Default|AutoBind|ManualBind|Break", (int*)(&Drag::TOOL));
    widget->addRadioGrp("Cont:","C0|C1|C2", (int*)(&Drag::CONT));
    return widget;
}

QWidget* createSetColorOptions()
{
    CustomDialog * widget = new CustomDialog("Set Color Tool Options",0,"Assign",exec, &SetColor::EXEC_ONCLICK);
    widget->addColorSel("Color",&SetColor::COLOR);
    widget->addRadioGrp("Apply:","Shape|Vertex", (int*)(&SetColor::SELECT_MODE));
    widget->addRadioGrp("Source:", "Tool|Dialog|Background", (int*)(&SetColor::SOURCE));
    //widget->addRadioGrp("Channel:","Dark|Bright|Depth", (int*)(&SetColor::CHANNEL));
    return widget;
}

QWidget* createSewOptions()
{
    CustomDialog * widget = new CustomDialog("Sew Tool Options",0,"Sew",exec);
    widget->addRadioGrp("Apply:","Vertex|Edge", (int*)(&Sew::SEW_MODE));
    return widget;
}

QWidget* createGridOptions()
{
    CustomDialog * widget = new CustomDialog("Grid Options",0, "Insert", createGrid);
    widget->addSpinBox("Rows:", 1, 8, &MeshPrimitive::GRID_N, 1, "Rows");
    widget->addSpinBox("Coloumns:", 1, 8, &MeshPrimitive::GRID_M, 1, "Coloumns");
    widget->addDblSpinBoxF("Rows    Length:", 0.01, 0.5, &MeshPrimitive::GRID_N_LEN, 2, 0.01, "");
    widget->addDblSpinBoxF("Columns Length:", 0.01, 0.5, &MeshPrimitive::GRID_M_LEN, 2, 0.01, "");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    widget->addRadioGrp("Surface:","ShapeMap|Grid Pattern|UV Pattern", (int*)(&MeshShape::PATCH_TYPE));
    return widget;
}

QWidget* createNgonOptions()
{
    CustomDialog * widget = new CustomDialog("2NGon Options", 0, "Insert", createNGon);
    widget->addRadioGrp("Policy:","Subdiv|Pie|Polygon", (int*)(&MeshPrimitive::NGON_HANDLING));
    widget->addSpinBox("Sides:", 1, 8, &MeshPrimitive::NGON_N, 1, "Sides of 2NGon");
    widget->addSpinBox("Segments:", 1, 4, &MeshPrimitive::NGON_SEG_V, 1, "Rows");
    widget->addDblSpinBoxF("Radius:", 0.01, 0.75, &MeshPrimitive::NGON_RAD, 2, 0.01, "");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    widget->addRadioGrp("Surface:","ShapeMap|Grid Pattern|UV Pattern", (int*)(&MeshShape::PATCH_TYPE));
    return widget;
}

QWidget* createTorusOptions()
{
    CustomDialog * widget = new CustomDialog("Torus Options",0, "Insert", createTorus);
    widget->addSpinBox("Sides:", 2, 24, &MeshPrimitive::TORUS_N, 1, "Sides of Torus");
    widget->addSpinBox("V Segments:", 1, 8, &MeshPrimitive::TORUS_V, 1, "Vertical Segments of The Torus");
    widget->addDblSpinBoxF("Radius:", 0.01, 1.0, &MeshPrimitive::TORUS_RAD_X, 2, 0.01, "");
    widget->addDblSpinBoxF("Radius:", 0.01, 1.0, &MeshPrimitive::TORUS_RAD_Y, 2, 0.01, "");
    widget->addDblSpinBoxF("Width%:", 0.01, 1.0, &MeshPrimitive::TORUS_W, 2, 0.01, "");
    widget->addDblSpinBoxF("Arc:", 0., 1.0, &MeshPrimitive::TORUS_ARC, 2, 0.01, "");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    widget->addRadioGrp("Surface:","ShapeMap|Grid Pattern|UV Pattern", (int*)(&MeshShape::PATCH_TYPE));

    return widget;
}

QWidget* createSpineOptions()
{
    CustomDialog * widget = new CustomDialog("Spine Options",0, "Insert", createSpine);
    widget->addDblSpinBoxF("Radius:", 0.01, 0.75, &SpineShape::RAD, 2, 0.01, "");
    widget->addCheckBox ("Triangulate Joints", &SpineShape::isTRIANGULATE_JOINTS,"");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    widget->addRadioGrp("Surface:","ShapeMap|Grid Pattern|UV Pattern", (int*)(&MeshShape::PATCH_TYPE));
    return widget;
}

QWidget* createAssignPatternOptions()
{
    CustomDialog * widget = new CustomDialog("Pattern Options",0, "Assign", executeMeshShapeOperations, &MeshOperation::EXEC_ONCLICK);
    widget->addLineEdit("pattern:",&PatternOperation::PATTERN);
    widget->addCheckBox ("Continuous", &PatternOperation::IS_CONT,"");
    return widget;
}

QWidget* createSetFoldsOptions()
{
    CustomDialog * widget = new CustomDialog("FoldingOptions",0, "Fold", executeMeshShapeOperations, &MeshOperation::EXEC_ONCLICK);
    widget->addSpinBox("Folds:", 0, 24, &PatternOperation::FOLD_N, 1, "Number of Folds");
    return widget;
}

QWidget* createExtrudeOptions()
{
    CustomDialog * widget = new CustomDialog("Extrusion Options",0, "Exture", executeMeshShapeOperations, &MeshOperation::EXEC_ONCLICK);
    widget->addDblSpinBoxF("Extrusion Amount:", 0, 0.5, &MeshOperation::EXTRUDE_T, 2, 0.01, "");
    widget->addCheckBox ("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    widget->addCheckBox ("Keep Faces Together", &MeshOperation::isKEEP_TOGETHER,"");
    widget->addCheckBox ("Cap Extrusion", &MeshOperation::EXTRUDE_CAP,"");
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

QWidget* createInsertSegmentOptions()
{
    CustomDialog * widget = new CustomDialog("InsertSegment Options");
    widget->addCheckBox("Keep Tangents Smooth", &MeshShape::isSMOOTH,"");
    return widget;
}

void createShapeImage(){
    //MeshShape* pMS =ImageShape::insertGrid(Point(), MeshShape::GRID_LEN, MeshShape::GRID_M, MeshShape::GRID_N);
#ifndef MODELING_MODE
    ImageShape* pIS = new ImageShape(1.0,1.0);
    //Session::get()->mainWindow()->addAttrWidget(createImageShapeAttrWidget(pIS ), (void*)pIS);

    ShapeOrder* so = new ShapeOrder(ShapeOrder::INSERT_SHAPE);
    so->setShape(pIS );
    Session::get()->exec(so);
#endif
}

QWidget* createImageShapeOptions()
{
    CustomDialog * widget = new CustomDialog("Image Shape Options", 0, "Insert", createShapeImage);
    return widget;
}

QWidget* createOrderFaceOptions()
{
    CustomDialog * widget = new CustomDialog("Face Order Options");
    widget->addCheckBox("Move Up", &OrderFace::isUP,"");
    return widget;
}

void MainWindow::createAllOptionsWidgets()
{
    addOptionsWidget(new QWidget(), Options::NONE);
    addOptionsWidget(createDragOptions(), Options::DRAG);
    addOptionsWidget(createGridOptions(), Options::GRID);
    addOptionsWidget(createNgonOptions(), Options::NGON);
    addOptionsWidget(createTorusOptions(), Options::TORUS);
    addOptionsWidget(createSpineOptions(), Options::SPINE);
    addOptionsWidget(createExtrudeOptions(), Options::EXTRUDE);
    addOptionsWidget(createInsertSegmentOptions(), Options::INSERT_SEGMENT);
    addOptionsWidget(createAssignPatternOptions(), Options::ASSIGN_PATTERN);
    addOptionsWidget(createSetFoldsOptions(), Options::SET_FOLDS);
    addOptionsWidget(createImageShapeOptions(), Options::IMAGE_SHAPE);
    addOptionsWidget(createSetColorOptions() , Options::SET_COLOR);
    addOptionsWidget(createSewOptions() , Options::SEW);
    addOptionsWidget(createOrderFaceOptions() , Options::ORDER_FACE);
}
