#include "meshshape.h"
#include "../curve.h"
#include "../commands.h"
#include "curvededge.h"

MeshShape::MeshShape()
{
    _control = new Mesh();
    _control->setInsertEdgeCB(onInsertEdge);
    _control->setAddFaceCB(onAddFace);
    _control->setUnsplitEdgeCB(onUnsplitEdge);
    _control->setCaller((void*)this);
}

Vertex_p MeshShape::addMeshVertex(ShapeVertex_p sv){
    if (!sv)
        sv = addVertex();
    Vertex_p v = _control->addVertex(sv);
    sv->setRef(v);
    return v;
}

Vertex_p MeshShape::addMeshVertex(const Point& p){
    Vertex_p v = addMeshVertex();
    v->pData->pP()->set(p);
    return v;
}

void MeshShape::onDrag(ShapeVertex_p pSV, const Vec2& t){

    if (Drag::TOOL == Drag::BREAK){
        pSV->unpair();
    }

    if (Drag::TOOL == Drag::AUTO_BIND && pSV->parent() && !pSV->pair())
    {
        Corner_p pC = ((Edge_p)pSV->ref())->pData->getCornerByTan(pSV);
        makeSmoothCorners(pC, false, 0);
    }
}
