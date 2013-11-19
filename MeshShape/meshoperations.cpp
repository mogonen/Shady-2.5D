#include "meshshape.h"

bool MeshShape::EXEC_ONCLICK = true;

//all operations on meshshape needs to be made static to allow operation on all layers
void MeshShape::execOP(const Point &p, Selectable_p obj){

    if (!obj->pRef)
        return;

    Edge_p pE = 0;
    Face_p pF = 0;
    MeshShape* pMS = 0;

    //there might be a better way for this
    if (_OPMODE == EXTRUDE_EDGE || _OPMODE == INSERT_SEGMENT){
         pE = dynamic_cast<Edge_p>((Edge_p)obj->pRef);
         if (!pE) return;
         pMS = ((MeshShape*)pE->mesh()->caller());
    }else if (_OPMODE == EXTRUDE_FACE || _OPMODE == DELETE_FACE){
         pF = dynamic_cast<Face_p>((Face_p)obj->pRef);
         if(!pF) return;
         pMS = ((MeshShape*)pF->mesh()->caller());
    }

    if (!pMS) return; //this is kinda redundant

    switch(_OPMODE){

        case MeshShape::NONE:
        break;

        case MeshShape::EXTRUDE_EDGE:
                pMS->extrude(pE, EXTRUDE_T);
        break;

        case MeshShape::INSERT_SEGMENT:
                pMS->insertSegment(pE, p);
        break;

        case MeshShape::EXTRUDE_FACE:
                pMS->extrude(pF, EXTRUDE_T);
        break;

        case MeshShape::DELETE_FACE:
                 pMS->deleteFace(pF);
        break;

    }

    if (_OPMODE != MeshShape::DELETE_FACE)
        pMS->Renderable::update();

}

void MeshShape::insertSegment(Edge_p e, const Point & p){

    if (!e )
        return;

    Mesh_p pMesh = e->mesh();

    list<Corner_p> clist;

    double t = 0.5;
    Corner* c0 = pMesh->splitEdge(e, addMeshVertex());
    onSplitEdge(c0, t);

    Corner* c1 = c0->vNext();
    Face_p endf = (c1)?c1->F():0;

    while(c0 && c0->F()!=endf){

        Corner* c01 = pMesh->splitEdge(c0->next()->next()->E(), addMeshVertex() ,c0->F());
        onSplitEdge(c01, (1-t));
        Corner* c0n = c01->vNext();
        pMesh->insertEdge(c0, c01);
        clist.push_back(c0);
        c0 = c0n;
    }

    if (c0 && c0->F() == endf){
        (pMesh->insertEdge(c0, c0->next()->next()->next()));
    }else while(c1){
        Corner* c11 = pMesh->splitEdge(c1->next()->next()->E(), addMeshVertex(), c1->F());
        onSplitEdge(c11, 1-t);
        Corner* c1n = c11->vNext();
        pMesh->insertEdge(c1, c11);
        clist.push_back(c1);
        c1 = c1n;
    }

    /*FOR_ALL_ITEMS(list<Corner_p>, clist)
            adjustInsertedSegmentTangents(*it);

    /*if (MeshShape::isSMOOTH){
        FOR_ALL_ITEMS(list<Corner_p>, clist){
            makeSmoothTangents(*it);
            makeSmoothTangents((*it)->vNext());
        }
    }*/

}

void MeshShape::diagonalDivide(Corner_p c){
    if (!c)
        return;
    Mesh_p pMesh = c->V()->mesh();
    Edge_p e = pMesh->insertEdge(c, c->next()->next(), true);
}

Face_p MeshShape::extrude(Face_p f0, double t){

    Mesh_p pMesh = f0->mesh();
    f0->update();
    Face_p f1 = pMesh->addFace(f0->size());

    Point pmid;//init as centroid of P

     for(int i=0; i<f0->size(); i++){
         pmid = pmid + P0(f0->C(i));
     }
     pmid = pmid*(1.0/f0->size());

    for(int i=0; i<f0->size(); i++)
    {
        Point p = P0(f0->C(i)) * (1-t) + pmid * t;
        f1->set(addMeshVertex(p), i);
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

        f0->C(i)->E()->set(f_side->C(0), f0->C(i)->isC0()?0:1);

        Edge_p e1 = pMesh->addEdge(f_side->C(1),0);
        Edge_p e2 = pMesh->addEdge(f1->C(i), f_side->C(2));

        if (e3)
            e3->set(f_side->C(3),1);
        else
            f_side_0  = f_side;

        e3 = e1;
    }

    e3->set(f_side_0->C(3),1);
    f1->update();

    if (isSMOOTH){
        for(int i=0; i<f1->size(); i++){
            Corner_p pC = f1->C(i);
            ShapeVertex_p sv = pC->E()->pData->getTangentSV(pC);
            sv->setPair(pC->prev()->E()->pData->getTangentSV(pC));
            Vec2 tan = P0(pC->next()) - P0(pC->prev());
            sv->setTangent(tan/6.0, false, true);
        }
    }
    pMesh->remove(f0);
    //*f0 = *f_side_0; //replace 1st side face with the old face
    return f1;
}

Edge_p MeshShape::extrude(Edge_p e0, double t){

    if (!e0 || !e0->isBorder())
        return 0;

    Mesh_p pMesh = e0->mesh();

    Face_p f = pMesh->addFace(4);

    //set verts
    Vec2 n = ( Vec3(0,0,1) % Vec3(P1(e0) - P0(e0)) ).normalize()*t;
    Vertex_p v0 = addMeshVertex(P0(e0)+n);
    Vertex_p v1 = addMeshVertex(P1(e0)+n);

    f->set(e0->C0()->next()->V(), 0);
    f->set(e0->C0()->V(), 1);
    f->set(v0, 2);
    f->set(v1, 3);

    //insert edges
    e0->set(f->C(0));
    pMesh->addEdge(f->C(1), 0); //e1
    Edge_p e2 = pMesh->addEdge(f->C(2), 0);
    pMesh->addEdge(f->C(3), 0); //e3

    f->Face::update();
    if (isSMOOTH){
        makeSmoothTangents(e2->C0());
        makeSmoothTangents(e2->C0()->next());
    }

    return e2;
}

void MeshShape::deleteFace(Face_p f){

    if (!f)
        return;

    for(int i=0; i<f->size(); i++){
        if (f->C(i)->E()->isBorder() && f->C(i-1)->E()->isBorder()){
            removeVertex(f->C(i)->V()->pData);
        }
        if (f->C(i)->E()->isBorder()){
            removeVertex(f->C(i)->E()->pData->pSV[1]);
            removeVertex(f->C(i)->E()->pData->pSV[2]);
            delete f->C(i)->E()->pData->pCurve;
        }
    }

    Mesh_p mesh = f->mesh();
    mesh->remove(f);
    if (mesh->sizeF()==0){
        Session::get()->removeShape((Shape_p)mesh->caller());
    }

}
