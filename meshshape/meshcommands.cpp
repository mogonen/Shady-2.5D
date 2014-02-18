#include "meshshape.h"
#include "meshcommands.h"
#include "spineshape.h"
#include "patch.h"

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

    if (!pMS) return; //assert
    _pMS = pMS;
    _pF = pF;
    _pE = pE;

    switch(_operation){

    case NONE:
        break;

    case EXTRUDE_EDGE:
        extrude(pE, EXTRUDE_T, MeshShape::isSMOOTH, isKEEP_TOGETHER?&vertexToCornerMap:0, &_cache);
        break;

    case INSERT_SEGMENT:
        insertSegment(pE, (_click.P - _pMS->P()), &_cache);
        break;

    case EXTRUDE_FACE:
        extrude(pF, EXTRUDE_T,  &_cache);
        break;

    case DELETE_FACE:
        deleteFace(pF, &_cache);
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
    _cache.restore();
    return 0;
}

void MeshOperation::onClick(const Click & click)
{
    if (!click.is(Click::UP) || !EXEC_ONCLICK)
        return;
    _click = click;
    Session::get()->exec(); //this could be improved later
}

void MeshOperationCache::restore(){

    for(list<Edge_p>::reverse_iterator it = _edgesToDel.rbegin(); it!=_edgesToDel.rend(); it++)
    {
        Edge_p e = (*it);
        for(int i=0; i<4; i++){
            e->pData->pSV[i]->_isDeleted = true;
        }
        e->mesh()->remove(e, true, false);
    }

    for(list<Face_p>::reverse_iterator it = _facesToDel.rbegin(); it!=_facesToDel.rend(); it++)
    {
        MeshOperation::deleteFace(*it);
    }

    for(list<FaceCache>::reverse_iterator it = _cachedFaces.rbegin(); it!=_cachedFaces.rend(); it++)
    {
        Face_p pF = (*it).F();
        pF->mesh()->restore(*it);
        for(int i=0 ;i<pF->size(); i++)
        {
            pF->C(i)->E()->pData->relink(pF->C(i)->E());
        }
        pF->pData->pSurface->outdate();
    }

    for(list<SVCache>::reverse_iterator it = _cachedSV.rbegin(); it!=_cachedSV.rend(); it++)
    {
        (*it).restore();
    }
}

void MeshOperationCache::add(Face_p pF, bool isdel){
    if (isdel)
        _facesToDel.push_back(pF);
    else{
        _cachedFaces.push_back(FaceCache(pF));
        for(int i=0; i<pF->size(); i++){
            _cachedSV.push_back(SVCache(pF->C(i)->E()->pData->pSV[1]));
            _cachedSV.push_back(SVCache(pF->C(i)->E()->pData->pSV[2]));
            _cachedSV.push_back(SVCache(pF->C(i)->V()->pData));
        }
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
        for(int i=0; i<4; i++)
            _cachedSV.push_back(SVCache(pE->pData->pSV[i]));
    }
}

void MeshPrimitive::onClick(const Click & click){
    if (!click.is(Click::DOWN))
        return;
    Session::get()->exec();
}

Command_p MeshPrimitive::exec(){

    Shape* pShape = 0;

    switch(_primitive){
    case GRID:
        pShape = insertGrid(Point(),GRID_M_LEN, GRID_N_LEN, GRID_M, GRID_N);
    break;

    case TWO_NGON:
        pShape = insertNGon(Point(), NGON_N, NGON_SEG_V, NGON_RAD);
    break;

    case TORUS:
        pShape = insertTorus(Point(), TORUS_N, TORUS_V, TORUS_RAD, TORUS_W, TORUS_ARC);
    break;

    case SPINE:
        pShape = new SpineShape();
    break;

    }

    Session::get()->insertShape(pShape);
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
