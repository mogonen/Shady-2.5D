#include "meshshape.h"
#include "../curve.h"
#include "../commands.h"

MeshShape::MeshShape()
{
    _control = new Mesh();
    _control->setInsertEdgeCB(onInsertEdge);
    _control->setAddFaceCB(onAddFace);
    _control->setCaller((void*)this);
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

void MeshShape::onDrag(ShapeVertex_p pSV, const Vec2& t){

    if (Drag::TOOL != Drag::AUTO_BIND || !pSV->parent() || pSV->pair() )//|| !(((Edge_p)pSV->pRef)->isBorder()))
        return;

    Corner_p pC = EdgeData::StaticGetCornerByTangent(pSV, true);
    makeSmoothCorners(pC, false, 0);
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
