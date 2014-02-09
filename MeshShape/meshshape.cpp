#include "meshshape.h"
#include "../curve.h"

MeshShape::MeshShape()
{
    _control = new Mesh();
    _control->setInsertEdgeCB(onInsertEdge);
    _control->setAddFaceCB(onAddFace);
    _control->setCaller((void*)this);
}

void MeshShape::onClick(const Point & p, Click_e eClick)
{

    if (!EXEC_ONCLICK)
        return;

    if (eClick == UP)
    {
        Selectable_p obj = Session::get()->selectionMan()->getLastSelected();

        if (!obj || obj->type() != Renderable::SHAPE)
            return;

        execOP(p, obj);
    }
}

void MeshShape::setOPMODE(OPERATION_e eMode){
    _OPMODE = eMode;
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
    static const SELECTION_e SELECTMODE[] = {NOSELECT, EDGE, FACE, FACE, CORNER, EDGE, NOSELECT};
    if (_OPMODE > 6)
        return NOSELECT;
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
