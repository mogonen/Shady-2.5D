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

double      MeshOperation::EXTRUDE_T        = 0.25;
bool        MeshOperation::isKEEP_TOGETHER  = false;
bool        MeshOperation::EXEC_ONCLICK     = true;




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

void MeshOperation::execOP(){

    switch(_operation){

    case NONE:
        break;

    case EXTRUDE_EDGE:
        extrude(_pE, EXTRUDE_T, MeshShape::isSMOOTH, isKEEP_TOGETHER?&vertexToCornerMap:0, &_cache);
        break;

    case INSERT_SEGMENT:
        insertSegment(_pE, (_click.P - _pMS->P()), &_cache);
        break;

    case EXTRUDE_FACE:
        extrude(_pF, EXTRUDE_T,  &_cache);
        break;

    case DELETE_FACE:
        deleteFace(_pF, &_cache);
        break;

    //Pattern Operatioms here for now
    case ASSIGN_PATTERN:
    {
       PatternOperation::assignPattern(_pE, PatternOperation::PATTERN);
    }
        break;

    case SET_FOLDS:
    {
        PatternOperation::setFolds(_pE, PatternOperation::FOLD_N, PatternOperation::FOLD_W);
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
    _cache.addMesh(_pMS->mesh());
    execOP();
    _pMS->Renderable::update();
    return new MeshOperation(_operation); // keep it going
}

Command_p MeshOperation::unexec(){
    _cache.restore();
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

void MeshOperationCache::restore(){
    _pMesh->rollback(_rollback_id);
    /*
    for(list<SVCache>::reverse_iterator it = _cachedSV.rbegin(); it!=_cachedSV.rend(); it++)
    {
        (*it).restore();
    }*/
}

void MeshOperationCache::add(Face_p pF, bool isdel){
    if (!pF)
        return;
    if (isdel)
        _facesToDel.push_back(pF);
    else{
        _cachedFaces.push_back(FaceCache(pF));
       /* for(int i=0; i<pF->size(); i++){
            _cachedSV.push_back(SVCache(pF->C(i)->E()->pData->pSV[1]));
            _cachedSV.push_back(SVCache(pF->C(i)->E()->pData->pSV[2]));
            _cachedSV.push_back(SVCache(pF->C(i)->V()->pData));
        }*/
    }
}

void MeshOperationCache::add(ShapeVertex_p pSV, bool isdel){
    if (!isdel)
        _cachedSV.push_back(SVCache(pSV));
}

void MeshOperationCache::add(Edge_p pE, bool isdel){
    if (isdel)
        _edgesToDel.push_back(pE);
    else{
        /*for(int i=0; i<4; i++)
            _cachedSV.push_back(SVCache(pE->pData->pSV[i]));*/
    }
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
        _pShape = insertTorus(Point(), TORUS_N, TORUS_V, TORUS_RAD, TORUS_W, TORUS_ARC);
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


/*
void MeshShape::executeStackOperation(){
    vertexToCornerMap.clear();
    SelectionSet selects = Session::get()->selectionMan()->getSelection();
    FOR_ALL_ITEMS(SelectionSet, selects){
        MeshShape::execOP(Point(),*it);
    }
}
*/
