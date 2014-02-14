#include "meshshape.h"
#include "meshcommands.h"

//Defaults
bool        MeshShape::isSMOOTH             = true;

double      MeshOperation::EXTRUDE_T        = 0.25;
int         MeshOperation::FOLD_N           = 8;
double      MeshOperation::FOLD_D           = 0;

bool        MeshOperation::isKEEP_TOGETHER  = false;
bool        MeshOperation::EXEC_ONCLICK     = true;
string      MeshOperation::PATTERN          = "1,2";



double      MeshPrimitive::GRID_N_LEN       = 0.2;
double      MeshPrimitive::GRID_M_LEN       = 0.2;

int         MeshPrimitive::GRID_N           = 2;
int         MeshPrimitive::GRID_M           = 2;
int         MeshPrimitive::NGON_N           = 3;
int         MeshPrimitive::NGON_SEG_V       = 1;
double      MeshPrimitive::NGON_RAD         = 0.2;

int         MeshPrimitive::TORUS_N          = 8;
int         MeshPrimitive::TORUS_V          = 1;
double      MeshPrimitive::TORUS_RAD        = 0.2;
double      MeshPrimitive::TORUS_W          = 0.5;
double      MeshPrimitive::TORUS_ARC        = 1.0;

std::map<Vertex_p, Corner_p> vertexToCornerMap;

void MeshOperation::execOP(Selectable_p obj){

    if (!obj->pRef)
        return;

    Edge_p pE = 0;
    Face_p pF = 0;
    MeshShape* pMS = 0;

    //there might be a better way for this
    if (_operation == EXTRUDE_EDGE || _operation == INSERT_SEGMENT || _operation == ASSIGN_PATTERN || _operation == SET_FOLDS){
         pE = dynamic_cast<Edge_p>((Edge_p)obj->pRef);
         if (!pE) return;
         pMS = ((MeshShape*)pE->mesh()->caller());
         _pE = pE;
    }else if (_operation == EXTRUDE_FACE || _operation == DELETE_FACE){
         pF = dynamic_cast<Face_p>((Face_p)obj->pRef);
         if(!pF) return;
         pMS = ((MeshShape*)pF->mesh()->caller());
    }

    if (!pMS) return; //this is kinda redundant

    _pMS = pMS;

    switch(_operation){

    case NONE:
        break;

    case EXTRUDE_EDGE:{
        pMS->extrude(pE, EXTRUDE_T, isKEEP_TOGETHER?&vertexToCornerMap:0);
        _pF = pE->C1()->F();
    }
        break;

    case INSERT_SEGMENT:
        pMS->insertSegment(pE, _click.P);
        break;

    case EXTRUDE_FACE:
        pMS->extrude(pF, EXTRUDE_T);
        break;

    case DELETE_FACE:
        pMS->deleteFace(pF);
        break;

    case ASSIGN_PATTERN:
    {
       //pMS->assignPattern(pE, PATTERN);
    }
        break;

    case SET_FOLDS:
    {
        //pMS->setFolds(pE, FOLD_N, FOLD_D);
    }
        break;

    }

    if (_operation != DELETE_FACE)
        pMS->Renderable::update();

}

//all operations on meshshape needs to be made static to allow operation on all layers
Command_p MeshOperation::exec(){
    Selectable_p obj = Session::get()->selectionMan()->getLastSelected();
    if (!obj || obj->isUI())
        return new MeshOperation(_operation);

    execOP(obj);
    return new MeshOperation(_operation); // keep it going
}

Command_p MeshOperation::unexec(){

    switch(_operation){

    case NONE:
        break;

    case EXTRUDE_EDGE:{
        _pMS->deleteFace(_pF);
    }
        break;

    case INSERT_SEGMENT:

        break;

    case EXTRUDE_FACE:

        break;

    case DELETE_FACE:

        break;

    case ASSIGN_PATTERN:
    {

    }
        break;

    case SET_FOLDS:
    {

    }
        break;

    }

    if (_operation != DELETE_FACE && _pMS)
        _pMS->Renderable::update();

    return new MeshOperation(_operation);
}

void MeshOperation::onClick(const Click & click)
{
    if (!click.is(Click::UP) || !EXEC_ONCLICK)
        return;
    _click = click;
    Session::get()->exec(); //this could be improved later
}


Command_p MeshPrimitive::exec(){

    MeshShape* pMS = 0;

    switch(_primitive){
    case GRID:
        pMS = MeshShape::insertGrid(Point(),GRID_M_LEN, GRID_N_LEN, GRID_M, GRID_N);
    break;

    case TWO_NGON:
        pMS = MeshShape::insertNGon(Point(), NGON_N, NGON_SEG_V, NGON_RAD);
    break;

    case TORUS:
        pMS = MeshShape::insertTorus(Point(), TORUS_N, TORUS_V, TORUS_RAD, TORUS_W, TORUS_ARC);
    break;

    case SPINE:

    break;

    }

    Session::get()->insertShape(pMS);
    return 0;
}

Command_p MeshPrimitive::unexec(){
    return 0;
}


/*
void MeshShape::executeStackOperation(){
    vertexToCornerMap.clear();
    SelectionSet selects = Session::get()->selectionMan()->getSelection();
    FOR_ALL_ITEMS(SelectionSet, selects){
        MeshShape::execOP(Point(),*it);
    }
}
*/
