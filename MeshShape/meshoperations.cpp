#include "meshcommands.h"

Bezier* initCurve(Corner_p pC){

    if(!pC)
        return 0;

    Edge_p pE = pC->E();

    if (!pE->pData)
        pE->pData = new EdgeData(pE);

    if (pE->pData->pCurve)
        return pE->pData->pCurve; //???

#if SHOW_DLFL

    return pE->pData->initCurve();

#else

    Point p0 = pC->V()->pData->P();
    Point p1 = pC->next()->V()->pData->P();
    Vec2 tan = p1 - p0;

    MeshShape* shape = (MeshShape*) pE->mesh()->caller();
    ShapeVertex_p sv0_t = shape->addVertex(p0 + tan/3.0, pC->V()->pData, true, false);
    ShapeVertex_p sv1_t = shape->addVertex(p1 - tan/3.0, pC->next()->V()->pData, true, false);

    return pE->pData->initCurve(sv0_t, sv1_t);

#endif
}

void onSplitEdge(Corner_p pC, double t)
{

    bool isforward = true;
    if (pC->isC1()){
        pC = pC->vNext();
        isforward = false;
    }

    if (pC->F())
        pC->F()->Face::update(true);
    if (pC->other()->F())
        pC->other()->F()->Face::update(true);

    //clean up
    Edge_p e0 = pC->prev()->E();

    if (e0->pData->pCurve->count() == 2 ){
        e0->pData->pCurve->set(pC->V()->pData->pP(),1);
        pC->V()->pData->pP()->set(pC->prev()->V()->pData->P()*0.5 + pC->next()->V()->pData->P()*0.5);
        return;
    }

    bool haspair =  e0->pData->pSV[1]->pair() || e0->pData->pSV[2]->pair();

    Point newCP[7];
    e0->pData->pCurve->computeSubdivisionCV(isforward? t:(1-t), newCP);

    e0->pData->discardCurve();
    e0->pData = new EdgeData(e0);
    e0->pData->initCurve();

    if (haspair)
        e0->pData->pSV[2]->setPair(pC->E()->pData->pSV[1]);

    //now assign new positions
    Bezier* curve0 = e0->pData->pCurve;

    curve0->pCV(1)->set(newCP[1]);
    curve0->pCV(2)->set(newCP[2]);
    curve0->pCV(3)->set(newCP[3]);

    Bezier* curve1 = pC->E()->pData->pCurve;

    curve1->pCV(1)->set(newCP[4]);
    curve1->pCV(2)->set(newCP[5]);

}

void onUnsplitEdge(Corner_p pC){
    pC->E()->pData->relink(pC->E());
}

Point computeVerticalTangent(double t, Edge_p pE, Face_p pF =0)
{
    bool isforward = (!pF) || (pF == pE->C0()->F());

    Point p[8];
    p[0] = pE->pData->pCurve->CV(0);
    p[1] = pE->pData->pCurve->CV(1);
    p[2] = pE->pData->pCurve->CV(2);
    p[3] = pE->pData->pCurve->CV(3);

    if (isforward){

        p[4] = pE->C0()->prev()->E()->pData->getTangentSV(pE->C0())->P();
        p[7] = pE->C0()->next()->E()->pData->getTangentSV(pE->C0()->next())->P();

    }else{
        p[4] = pE->C1()->next()->E()->pData->getTangentSV(pE->C1()->next())->P();
        p[7] = pE->C1()->prev()->E()->pData->getTangentSV(pE->C1())->P();
        t=1-t;
    }

    p[5] = p[4] + p[1] - p[0];
    p[6] = p[7] + p[2] - p[3];

    Point p45 = p[4]*(1-t) + p[5]*(t);
    Point p56 = p[5]*(1-t) + p[6]*(t);
    Point p67 = p[6]*(1-t) + p[7]*(t);

    Point p0 = p45*(1-t) + p56*t;
    Point p1 = p56*(1-t) + p67*t;
    return p0*(1-t) + p1*t;
}


void MeshOperation::insertSegment(Edge_p e, const Point & p,  MeshOperationCache *pCache){

    if (!e )
        return;

    Mesh_p pMesh = e->mesh();
    MeshShape* pMS = (MeshShape*)pMesh->caller();

    double t;
    e->pData->pCurve->computeDistance(p, t);

    Point tan0 = computeVerticalTangent(t, e);
    Corner_p c0 = pMesh->splitEdge(e->C0(), pMS->addMeshVertex());
    onSplitEdge(c0, t);

    Corner_p c1 = c0->vNext();
    Face_p endf = (c1)?c1->F():0;

    while(!c0->isBorder() && c0->F()!=endf){

        Point tan1  = computeVerticalTangent((1-t), c0->next()->next()->E(), c0->F());
        Point tan00 = computeVerticalTangent(t, c0->next()->next()->E(), c0->next()->next()->vNext()->F());

        Corner* c01 = pMesh->splitEdge(c0->next()->next(), pMS->addMeshVertex());
        onSplitEdge(c01, 1-t);
        Edge_p e_c01 = c01->E();

        Corner* c0n = c01->vNext();
        Edge_p pEnew = pMesh->insertEdge(c0, c01);

        pEnew->pData->pSV[1]->pP()->set(tan0);
        pEnew->pData->pSV[2]->pP()->set(tan1);
        tan0 = tan00;

        c0 = c0n;
    }

    if (!c0->isBorder() && c0->F() == endf){ //looping
        Edge_p pEnew = (pMesh->insertEdge(c0, c0->next()->next()->next()));
         if (pCache)
             pCache->add(pEnew, true);
    }else while(!c1->isBorder()){
        Corner* c11 = pMesh->splitEdge(c1->next()->next(), pMS->addMeshVertex());
        onSplitEdge(c11, t);
        Edge_p e_c11 = c11->E();

        Corner* c1n = c11->vNext();
        Edge_p pEnew = pMesh->insertEdge(c1, c11);

        c1 = c1n;
    }

}

void MeshOperation::diagonalDivide(Corner_p c){
    if (!c)
        return;
    Mesh_p pMesh = c->V()->mesh();
    Edge_p e = pMesh->insertEdge(c, c->next()->next(), true);
}

Face_p MeshOperation::extrude(Face_p f0, double t, MeshOperationCache *pCache){

    Mesh_p pMesh = f0->mesh();
    MeshShape* pMS = (MeshShape*)pMesh->caller();
    f0->update();

    Point pmid;//init as centroid of P
    Corner_p corns[8];
    for(int i=0; i<f0->size(); i++){
        pmid = pmid + P0(f0->C(i));
        corns[i] = f0->C(i);
    }

    pmid = pmid*(1.0/f0->size());

    Edge_p e0, e00;
    e0 = e00 = 0;
    for(int i=0; i <f0->size(); i++)
    {
        Point p = P0(corns[i]) * (1-t) + pmid * t;
        Vertex_p vnew = pMS->addMeshVertex(p);
        Edge_p e1 = pMesh->insertEdge(corns[i], new Corner(vnew));
        if (e0)
           pMesh->insertEdge(e0->C1(), e1->C1());
        else
           e00 = e1;
        e0 = e1;
    }

    Edge_p e = pMesh->insertEdge(e0->C1(), e00->C0()->next());
    return e->C0()->F();
}

Edge_p MeshOperation::extrude(Edge_p e0, double t, bool isSmooth, VertexMap *pVMap, MeshOperationCache *pCache){

    if (!e0 || !e0->isBorder())
        return 0;

    Mesh_p pMesh = e0->mesh();
    MeshShape* pMS = (MeshShape*)pMesh->caller();

    Corner_p c0 = e0->C()->other();

    Point p0 = P0(c0);
    Point p1 = P0(c0->next());

    //set verts
    Vec2 n = -( Vec3(0,0,1) % Vec3(p1-p0).normalize())*t;

    Vertex_p v0 = pMS->addMeshVertex(p0+n);
    Vertex_p v1 = pMS->addMeshVertex(p1+n);

    Face_p f = pMesh->addQuad(c0, c0->next(),  new Corner(v1), new Corner(v0));
    f->update(true);
    f->reoffset(e0->C()->I() + 2 - e0->C()->other()->I());

    if (isSmooth && !pVMap){
        for(int i=0; i<4; i++)
            MeshShape::makeSmoothCorners(f->C(i),true, 1);
    }

    return 0;
}

void MeshOperation::deleteFace(Face_p f, MeshOperationCache *pCache){

    if (!f)
        return;

    if (pCache)
        pCache->add(f);

    MeshShape* pMS = (MeshShape*)(f->mesh()->caller());
    pMS->mesh()->remove(f);
   /*
    if (mesh->sizeF()==0)u
    {
        //Session::get()->removeShape((Shape_p)mesh->caller());
    }*/
}
