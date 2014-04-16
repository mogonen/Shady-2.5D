#include "meshshape.h"
#include "meshcommands.h"
#include "spineshape.h"
#include "patch.h"

//Defaults
#ifdef  SHOW_DLFL
bool        MeshShape::isSMOOTH             = false;
#else
bool        MeshShape::isSMOOTH             = true;
#endif

double          MeshOperation::EXTRUDE_T        = 0.25;
bool            MeshOperation::isKEEP_TOGETHER  = false;
bool            MeshOperation::EXEC_ONCLICK     = true;


double          MeshPrimitive::GRID_N_LEN       = 0.2;
double          MeshPrimitive::GRID_M_LEN       = 0.2;

int             MeshPrimitive::GRID_N           = 2;
int             MeshPrimitive::GRID_M           = 2;
int             MeshPrimitive::NGON_N           = 3;
int             MeshPrimitive::NGON_SEG_V       = 1;
double          MeshPrimitive::NGON_RAD         = 0.2;

int             MeshPrimitive::TORUS_N          = 4;
int             MeshPrimitive::TORUS_V          = 1;
double          MeshPrimitive::TORUS_RAD_X      = 0.2;
double          MeshPrimitive::TORUS_RAD_Y      = 0.2;

double          MeshPrimitive::TORUS_W          = 0.5;
double          MeshPrimitive::TORUS_ARC        = 1.0;

Sew::SewMode    Sew::SEW_MODE                   = Sew::SEW_EDGE;

std::map<Vertex_p, Corner_p> vertexToCornerMap;

void MeshOperation::execOP(){

    switch(_operation){

    case NONE:
        break;

    case EXTRUDE_EDGE:
        extrude(_pE, EXTRUDE_T, MeshShape::isSMOOTH, isKEEP_TOGETHER?&vertexToCornerMap:0);
        break;

    case INSERT_SEGMENT:
        insertSegment(_pE, (_click.P - _pMS->P()));
        break;

    case EXTRUDE_FACE:
        extrude(_pF, EXTRUDE_T);
        break;

    case DELETE_FACE:
        deleteFace(_pF);
        break;

    //Pattern Operatioms here for now
    case ASSIGN_PATTERN:
    {
       PatternOperation::assignPattern(_pE, PatternOperation::PATTERN, PatternOperation::IS_CONT);
    }
        break;

    case SET_FOLDS:
    {
        PatternOperation::setFolds(_pE, PatternOperation::FOLD_N, PatternOperation::FOLD_W, PatternOperation::IS_CONT);
    }
        break;

    }

}

//all operations on meshshape needs to be made static to allow operation on all layers

bool MeshOperation::pickElement(){
    _pE  = 0;
    _pF  = 0;
    _pMS = 0;
    Selectable_p obj = Session::get()->selectionMan()->getLastSelected();
    if (!obj || obj->isUI() || !obj->ref())
        return false;
    //there might be a better way for this
    if (_operation == EXTRUDE_EDGE || _operation == INSERT_SEGMENT || _operation == ASSIGN_PATTERN || _operation == SET_FOLDS){
        _pE = dynamic_cast<Edge_p>((Edge_p)obj->ref());
         if (!_pE)
             return false;
         _pMS = ((MeshShape*)_pE->mesh()->caller());
         return true;
    }else if (_operation == EXTRUDE_FACE || _operation == DELETE_FACE){
        _pF = dynamic_cast<Face_p>((Face_p)obj->ref());
         if(!_pF)
             return false;
         _pMS = ((MeshShape*)_pF->mesh()->caller());
         return true;
    }
    return false;
}

Command_p MeshOperation::exec(){
    _rollbackId = _pMS->mesh()->getOperationID();
    execOP();
    _pMS->Renderable::update();
    return new MeshOperation(_operation); // keep it going
}

Command_p MeshOperation::unexec(){
    _pMS->mesh()->rollback(_rollbackId);
    _pMS->update();
    return 0;
}

void MeshOperation::onClick(const Click & click)
{
    if (!click.is(Click::UP) || !EXEC_ONCLICK)
        return;
    _click = click;
    bool ispick = pickElement();
    if (ispick)
        Session::get()->exec(); //this could be improved later
}

void MeshPrimitive::onClick(const Click & click){
    if (!click.is(Click::DOWN))
        return;
    Session::get()->exec();
}

Command_p MeshPrimitive::exec(){

    _pShape = 0;

    switch(_primitive){
    case GRID:
        _pShape = insertGrid(Point(),GRID_M_LEN, GRID_N_LEN, GRID_M, GRID_N);
    break;

    case TWO_NGON:
        _pShape = insertNGon(Point(), NGON_N, NGON_SEG_V, NGON_RAD);
    break;

    case TORUS:
        _pShape = insertTorus(Point(), TORUS_N, TORUS_V, TORUS_RAD_X, TORUS_RAD_Y, TORUS_W, TORUS_ARC);
    break;

    case SPINE:
        _pShape = new SpineShape();
    break;

    }

    if (_pShape)
        Session::get()->insertShape(_pShape);
    return 0;
}

Command_p MeshPrimitive::unexec(){
    if (_pShape)
        Session::get()->removeShape(_pShape);
    return 0;
}


Sew::Sew()
{
}


Command_p Sew::exec(){

    //TODO::this stufff should bew done upper in command class
    if (Session::get()->selectionMan()->selectionSize()!=2)
        return new Sew();

    _sewmode = SEW_MODE;
    _svcount =0;

    SelectionSet selection = Session::get()->selectionMan()->getSelection();

    if (SEW_MODE == SEW_VERTEX){
        ShapeVertex_p   pSV[2];
        FOR_ALL_ITEMS(SelectionSet, selection)
        {
            pSV[(*it)->I()]= dynamic_cast<ShapeVertex_p>(*it);
        }

        if (!pSV[0] || !pSV[1])
            return new Sew();

        sewVertex(pSV[0], pSV[1]);
    }

    if (SEW_MODE == SEW_EDGE){
        CurvedEdge* pE[2];
        FOR_ALL_ITEMS(SelectionSet, selection)
        {
            pE[(*it)->I()]= dynamic_cast<CurvedEdge*>(*it);
        }

        if (!pE[0] || !pE[1])
            return new Sew();

        Vec2 v0 = pE[0]->getSV(3)->P() - pE[0]->getSV(0)->P();
        Vec2 v1 = pE[1]->getSV(3)->P() - pE[1]->getSV(0)->P();

        if (v0*v1 > 0){
            sewVertex(pE[0]->getSV(0),  pE[1]->getSV(0));
            sewVertex(pE[0]->getSV(1),  pE[1]->getSV(1));
            sewVertex(pE[0]->getSV(2),  pE[1]->getSV(2));
            sewVertex(pE[0]->getSV(3),  pE[1]->getSV(3));

        }else{
            sewVertex(pE[0]->getSV(0),  pE[1]->getSV(3));
            sewVertex(pE[0]->getSV(1),  pE[1]->getSV(2));
            sewVertex(pE[0]->getSV(2),  pE[1]->getSV(1));
            sewVertex(pE[0]->getSV(3),  pE[1]->getSV(0));
        }

    }

    return new Sew();

}

Command_p Sew::unexec(){
    for(int i=0; i<_svcount;i++)
        _cache[i].restore();

    return 0;
}

SelectionMode Sew::selectMode() const
{
    if (SEW_MODE == SEW_VERTEX)
        return SELECT_VERTEX_TANGENT;

    if (SEW_MODE == SEW_EDGE)
        return SELECT_EDGE;
}

void Sew::sewVertex(ShapeVertex_p sv0, ShapeVertex_p sv1){
    _cache[_svcount].set(sv0);
    sv0->setGlobalP(sv1->gP());
    for(int i=0; i< ACTIVE_CHANNELS; i++)
        sv0->value[i] = sv1->value[i];

    sv0->outdate();
    _svcount++;
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
