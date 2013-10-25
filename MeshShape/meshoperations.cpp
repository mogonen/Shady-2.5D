#include "meshshape.h"


void MeshShape::insertSegment(Edge_p e, const Point & p){

    if (!e || !e->isBorder())
        return;

    Mesh_p pMesh = e->mesh();

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
        c0 = c0n;
    }

    if (c0 && c0->F() == endf)
        (pMesh->insertEdge(c0, c0->next()->next()->next()));
    else while(c1){
        Corner* c11 = pMesh->splitEdge(c1->next()->next()->E(), addMeshVertex(), c1->F());
        onSplitEdge(c11, 1-t);
        Corner* c1n = c11->vNext();
         pMesh->insertEdge(c1, c11);
        c1 = c1n;
    }

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

    f->mesh()->remove(f);
}
