#include "meshcommands.h"

Bezier* initCurve(Corner_p pC){

    if(!pC)
        return 0;

    Edge_p pE = pC->E();

    if (!pE->pData)
        pE->pData = new EdgeData(pE);


    Vertex_p v0 = pC->V();
    Vertex_p v1 = pC->next()->V();

    if (pE->pData->pCurve)
        return pE->pData->pCurve;
    /*{
        EdgeData* pED = pE->pData;
        pED->pSV[0] = v0->pData;
        pED->pSV[3] = v1->pData;

        pED->pSV[0]->pRef = (void*)v0;
        pED->pSV[3]->pRef = (void*)v1;

        ShapeVertex_p sv1 = pED->getTangentSV(pC);
        ShapeVertex_p sv2 = pED->getTangentSV(pC->next());

        pED->pSV[1] = sv1;
        pED->pSV[2] = sv2;

        pED->pCurve->set(pED->pSV[0]->pP(),0);
        pED->pCurve->set(pED->pSV[1]->pP(),1);
        pED->pCurve->set(pED->pSV[2]->pP(),2);
        pED->pCurve->set(pED->pSV[3]->pP(),3);

    }*/

    Point p0 = v0->pData->P();
    Point p1 = v1->pData->P();

    Vec2 tan0 = p1 - p0;
    Vec2 tan1 = p0 - p1;

    MeshShape* shape = (MeshShape*) pE->mesh()->caller();

    ShapeVertex_p sv0_t = shape->addVertex(p0 + tan0/3.0, v0->pData, true, false);
    ShapeVertex_p sv1_t = shape->addVertex(p1 + tan1/3.0, v1->pData, true, false);

    sv0_t->pRef = (void*)pE;
    sv1_t->pRef = (void*)pE;

    pE->pData->pSV[1] = sv0_t;
    pE->pData->pSV[2] = sv1_t;

    pE->pData->pSV[0] = pC->V()->pData;
    pE->pData->pSV[3] = pC->next()->V()->pData;

    Bezier* c = new Bezier(100);
    pE->pData->pCurve = c;

    c->insert(v0->pData->pP());
    c->insert(sv0_t->pP());
    c->insert(sv1_t->pP());
    c->insert(v1->pData->pP());
    c->pRef = (void*) pE;
    return c;
}

void onSplitEdge(Corner_p pC, double t)
{

    bool isforward = true;
    if (pC->isC1()){
        pC = pC->vNext();
        isforward = false;
    }

    pC->F()->Face::update(true);
    if (!pC->E()->isBorder())
        pC->other()->F()->Face::update(true);

    //clean up
    Edge_p e0 = pC->prev()->E();
    ShapeVertex_p svtan = e0->pData->getTangentSV(pC->next());
    pC->V()->pData->adopt(svtan);
    if (svtan->pair()){
        svtan->pair()->setPair(pC->E()->pData->pSV[2]);
    }

    //parenting fixed
    //now adjust control p's
    Bezier* curve0 = e0->pData->pCurve;

    Point newCP[7];
    curve0->computeSubdivisionCV(isforward? t:(1-t), newCP);
    curve0->set(pC->V()->pData->pP(), 3);

    curve0->pCV(1)->set(newCP[1]);
    curve0->pCV(2)->set(newCP[2]);
    curve0->pCV(3)->set(newCP[3]);

    Bezier* curve1 = pC->E()->pData->pCurve;

    curve1->pCV(1)->set(newCP[4]);
    curve1->pCV(2)->set(newCP[5]);

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

    if (pCache){
        //need tp cache faces first
        Corner_p c0 = e->C0(), c1 = e->C1();
        bool isloop = false;
        while(c0){
            pCache->add(c0->F());
            c0 = c0->prev()->vPrev();
            if (c0 && c0->F() == e->C0()->F()){
                isloop = true;
                break;
            }
        }

        while(!isloop && c1){
            pCache->add(c1->F());
            c1 = c1->prev()->vPrev();
        }
    }

    Mesh_p pMesh = e->mesh();
    MeshShape* pMS = (MeshShape*)pMesh->caller();

    double t;
    e->pData->pCurve->computeDistance(p, t);

    Point tan0 = computeVerticalTangent(t, e);
    Corner_p c0 = pMesh->splitEdge(e->C0(), pMS->addMeshVertex());
    onSplitEdge(c0, t);

    Corner_p c1 = c0->vNext();
    Face_p endf = (c1)?c1->F():0;

    while(c0 && c0->F()!=endf){

        Point tan1  = computeVerticalTangent((1-t), c0->next()->next()->E(), c0->F());

        Corner_p c0nnvn = c0->next()->next()->vNext();
        Face_p f1 = c0nnvn ? c0nnvn->F():0;
        Point tan00 = computeVerticalTangent(t, c0->next()->next()->E(),f1);

        Corner* c01 = pMesh->splitEdge(c0->next()->next(), pMS->addMeshVertex());
        onSplitEdge(c01, 1-t);
        Edge_p e_c01 = c01->E();

        Corner* c0n = c01->vNext();
        Edge_p pEnew = pMesh->insertEdge(c0, c01);

        if (pCache){
            pCache->add(pEnew->C1()->F(), true);
            pCache->add(e_c01, true);
            pCache->add(pEnew, true);
        }
        pEnew->pData->pSV[1]->pP()->set(tan0);
        pEnew->pData->pSV[2]->pP()->set(tan1);
        tan0 = tan00;
        c0 = c0n;
    }

    if (c0 && c0->F() && c0->F() == endf){ //looping
        Edge_p pEnew = (pMesh->insertEdge(c0, c0->next()->next()->next()));
         if (pCache)
             pCache->add(pEnew, true);
    }else while(c1 && c1->F()){
        Corner* c11 = pMesh->splitEdge(c1->next()->next(), pMS->addMeshVertex());
        onSplitEdge(c11, t);
        Edge_p e_c11 = c11->E();

        Corner* c1n = c11->vNext();
        Edge_p pEnew = pMesh->insertEdge(c1, c11);

        if (pCache){
            pCache->add(pEnew->C1()->F(), true);
            pCache->add(e_c11, true);
            pCache->add(pEnew, true);
        }

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

    if (pCache)
        pCache->add(f0);

    Face_p f1 = pMesh->addFace(f0->size());

    Point pmid;//init as centroid of P

     for(int i=0; i<f0->size(); i++){
         pmid = pmid + P0(f0->C(i));
     }
     pmid = pmid*(1.0/f0->size());

    for(int i=0; i<f0->size(); i++)
    {
        Point p = P0(f0->C(i)) * (1-t) + pmid * t;
        f1->set(pMS->addMeshVertex(p), i);
    }

    Edge_p e3 = 0;
    Face_p f_side_0 = 0;
    for(int i=0; i<f1->size(); i++){
        Face_p f_side =  pMesh->addFace(4);

        //set verts
        f_side->set(f0->C(i)->V(), 0);
        f_side->set(f0->C(i+1)->V(), 1);
        f_side->set(f1->C(i+1)->V(), 2);
        f_side->set(f1->C(i)->V(), 3);
        f_side->Face::update();

        f0->C(i)->E()->set(f_side->C(0), f0->C(i)->isC1());

        Edge_p e1 = pMesh->addEdge(f_side->C(1),0);
        Edge_p e2 = pMesh->addEdge(f1->C(i), f_side->C(2));

        if (e3)
            e3->set(f_side->C(3),1);
        else
            f_side_0  = f_side;

        e3 = e1;

        if (pCache)
            pCache->add(f_side, true);
    }

    e3->set(f_side_0->C(3),1);
    f1->update();

    if (pCache)
        pCache->add(f1, true);

    pMesh->remove(f0);
    //*f0 = *f_side_0; //replace 1st side face with the old face
    return f1;
}

Edge_p MeshOperation::extrude(Edge_p e0, double t, bool isSmooth, VertexMap *pVMap, MeshOperationCache *pCache){

    if (!e0 || !e0->isBorder())
        return 0;

    Mesh_p pMesh = e0->mesh();
    MeshShape* pMS = (MeshShape*)pMesh->caller();

    Face_p f = pMesh->addFace(4);

    if (pCache){
        pCache->add(f, true);
        pCache->add(e0);
    }
    //set verts
    Vec2 n = ( Vec3(0,0,1) % Vec3(P1(e0) - P0(e0)) ).normalize()*t;

    Vertex_p v0 = 0;
    Vertex_p v1 = 0;

    Edge_p e1 = 0;
    Edge_p e3 = 0;

    /*if (pVMap){
        std::map<Vertex_p, Corner_p>::iterator it_v = pVMap->find(e0->C0()->V());
        if (it_v == pVMap->end()){
            v0 = pMS->addMeshVertex(P0(e0)+n);
            (*pVMap)[e0->C0()->V()] = f->C(1);
        }else{
            Corner_p ec = it_v->second;
            e1 = ec->E();
            v0 = (ec->V() == e0->C0()->V() || ec->V() == e0->C0()->next()->V())? ec->next()->V():ec->V();
            v0->pData->pP()->set(P0(e0) + ((v0->pData->P() + P0(e0)+n)*0.5).normalize()*t);
        }

        it_v = pVMap->find(e0->C0()->next()->V());
        if (it_v == pVMap->end()){
            v1 = pMS->addMeshVertex(P1(e0)+n);
            (*pVMap)[e0->C0()->next()->V()] = f->C(3);
        }else{
            Corner_p ec = it_v->second;
            e3 = ec->E();
            v1 = (ec->V() == e0->C0()->V() || ec->V() == e0->C0()->next()->V())? ec->next()->V():ec->V();
            v1->pData->pP()->set(P1(e0) + ((v1->pData->P() + P1(e0)+n)*0.5).normalize()*t);
        }

    }else{*/
    v0 = pMS->addMeshVertex(P0(e0)+n);
    v1 = pMS->addMeshVertex(P1(e0)+n);

    f->set(e0->C0()->next()->V(), 0);
    f->set(e0->C0()->V(), 1);
    f->set(v0, 2);
    f->set(v1, 3);

    //insert edges
    e0->set(f->C(0));
    if (e1)
        e1->set(f->C(1));
    else
        pMesh->addEdge(f->C(1), 0); //e1

    Edge_p e2 = pMesh->addEdge(f->C(2), 0);

    if (e3)
        e3->set(f->C(3));
    else
        pMesh->addEdge(f->C(3), 0); //e3

    f->Face::update(false, e0->C0()->I()+2);

    if (isSmooth && !pVMap){
        for(int i=0; i<4; i++)
            MeshShape::makeSmoothCorners(f->C(i),true, 1);
    }

    return e2;
}

void MeshOperation::deleteFace(Face_p f, MeshOperationCache *pCache){

    if (!f)
        return;

    if (pCache)
        pCache->add(f);

    MeshShape* pMS = (MeshShape*)(f->mesh()->caller());

    for(int i=0; i<f->size(); i++){
        if (f->C(i)->E()->isBorder() && f->C(i-1)->E()->isBorder()){
            pMS->removeVertex(f->C(i)->V()->pData);
        }
        if (f->C(i)->E()->isBorder()){
            pMS->removeVertex(f->C(i)->E()->pData->pSV[1]);
            pMS->removeVertex(f->C(i)->E()->pData->pSV[2]);
            //delete f->C(i)->E()->pData->pCurve;
        }
    }

    pMS->mesh()->remove(f);
    /*if (mesh->sizeF()==0)u
    {
        //Session::get()->removeShape((Shape_p)mesh->caller());
    }*/
}
