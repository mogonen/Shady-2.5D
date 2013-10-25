#include "meshshape.h"
#include "../curve.h"

double MeshShape::EXTRUDE_T = 0.25;
MeshShape::OPERATION_e MeshShape::_OPMODE = MeshShape::EXTRUDE_EDGE;
bool MeshShape::isSMOOTH = true;

MeshShape::MeshShape(Mesh_p control)
{
    _control = control;
    if (!_control){ 
        //insert a quad face only
        _control = new Mesh();
        _control->setInsertEdgeCB(onInsertEdge);
        _control->setAddFaceCB(onAddFace);
        _control->setCaller((void*)this);
    }
}

void MeshShape::onClick(const Point & p, Click_e eClick){

    if (eClick == UP){
        execOP(p);
    }
}


void MeshShape::setOPMODE(OPERATION_e eMode){
    _OPMODE = eMode;
}

//all operations on meshshape needs to be made static to allow operation on all layers
void MeshShape::execOP(const Point &p){

    Selectable_p obj = Selectable::getLastSelected();

    if (!obj || obj->type() != Renderable::SHAPE)
        return;

    Edge_p e = (Edge_p)obj->pRef;
    Face_p f = (Face_p)obj->pRef;

    MeshShape* pMS = 0;

    //there might be a better way for this
    if (_OPMODE == EXTRUDE_EDGE || _OPMODE == INSERT_SEGMENT){
         pMS = ((MeshShape*)e->mesh()->caller());
    }else if (_OPMODE == EXTRUDE_FACE || _OPMODE == DELETE_FACE){
         pMS = ((MeshShape*)f->mesh()->caller());
    }

    if ( !e && !f)
        return;

    switch(_OPMODE){

        case MeshShape::NONE:
        break;

        case MeshShape::EXTRUDE_EDGE:
            if (e)
                pMS->extrude(e, EXTRUDE_T);
        break;

        case MeshShape::INSERT_SEGMENT:
            if (e)
                pMS->insertSegment(e, p);
        break;

        case MeshShape::EXTRUDE_FACE:
            if (f)
                pMS->extrude(f, EXTRUDE_T);
        break;

        case MeshShape::DELETE_FACE:
            if (f)
                pMS->deleteFace(f);
        break;

    }

    if (pMS)
        pMS->Renderable::update();

}

Vertex_p MeshShape::addMeshVertex(){
    ShapeVertex_p sv = addVertex();
    Vertex_p v = _control->addVertex(sv);
    sv->pRef = v;
    return v;
}

Vertex_p MeshShape::addMeshVertex(const Point& p){
    Vertex_p v = addMeshVertex();
    v->pData->P.set(p);
    return v;
}

MeshShape::SELECTION_e MeshShape::GetSelectMode(){
    static const SELECTION_e SELECTMODE[] = {NOSELECT, EDGE, FACE, FACE, CORNER, EDGE};
    return SELECTMODE[(int)_OPMODE];
}

/*
MeshShape* MeshShape::newMeshShape(const Point&p, PRIMITIVE_e ePrim)
{

    MeshShape* pMS = new MeshShape();

    switch(ePrim)
    {
        case SQUARE:
        {
            Vertex_p verts[4];
            Point ps[4];
            ps[0].set(-0.1, 0.1);
            ps[1].set(0.1, 0.1);
            ps[2].set(0.1, -0.1);
            ps[3].set(-0.1, -0.1);

            for(int i=0; i<4; i++) {
                verts[i] = pMS->addMeshVertex(ps[i]);
            }

            pMS->_control->addQuad(verts[0], verts[1], verts[2], verts[3]);
            pMS->_control->buildEdges();
        }
        break;

        case GRID:
        {
            insertGrid(p, 0.1, 2, 2, pMS);
        }
        break;

        case NGON:
        {
            insertNGon(p, 2, 0.1,pMS);
        }
        break;
    }

    pMS->Renderable::update();
    return pMS;
}*/
