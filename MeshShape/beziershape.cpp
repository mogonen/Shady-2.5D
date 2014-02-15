#include "meshshape.h"
#include "../curve.h"
#include "Patch.h"
#include "MeshData.h"

void  MeshShape::outdate(ShapeVertex_p sv){
    if (sv->parent()){ //if tagent
        Edge_p e = (Edge_p)(sv->pRef);
        if (!e)
            return;
        e->pData->pCurve->update();
        e->C0()->F()->pData->pSurface->outdate();
        if (e->C1())
            e->C1()->F()->pData->pSurface->outdate();

    }else{//this is vertex
        Vertex_p v = (Vertex_p)(sv->pRef);
        if (!v)
            return;
        Corner_p c = v->C();
        if (!c)
            return;
        Corner_p c0 = c;
        while(c){
            c->F()->pData->pSurface->outdate();
            c->E()->pData->pCurve->outdate();
            c = c->vNext();
            if (c==c0)
                break;
        }

        if (c == c0)
            return;

        Corner_p clast = c0;
        c = c0->vPrev();

        while(c){
            c->F()->pData->pSurface->outdate();
            c->E()->pData->pCurve->outdate();
            clast = c;
            c = c->vPrev();
            if (c==c0)
                break;
        }

        if (!c && clast)
            clast->prev()->E()->pData->pCurve->outdate();

    }
}

void MeshShape::onUpdate(){

    EdgeList edges = _control->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        Edge_p e = (*it);
        if (e->pData->pCurve)
            e->pData->pCurve->update();
    }

    FaceList faces = _control->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        if ((*it)->pData->pSurface)
            (*it)->pData->pSurface->update();
    }
}

void MeshShape::onEnsureUpToDate(){

    EdgeList edges = _control->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        Edge_p e = (*it);
        if (e->pData->pCurve)
            e->pData->pCurve->ensureUpToDate();
    }

    FaceList faces = _control->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        if ((*it)->pData->pSurface)
            (*it)->pData->pSurface->ensureUpToDate();
    }
}

void MeshShape::makeSmoothTangents(bool isskip, int ttype, double tank){

    EdgeList edges = _control->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        Edge_p e = (*it);
        makeSmoothCorners(e->C0(), isskip, ttype, tank);
        //makeSmoothCorners(e->C1(), isskip, ttype, tank);
        //makeSmoothTangents(e->C0());
    }
}

void MeshShape::makeSmoothTangents(Corner_p pC){

    Corner_p c0 = pC; //pC->isC0()?pC:pC->next()->vPrev();

    Corner_p vprev = c0->vPrev();
    Corner_p vnext = c0->vNext();

    ShapeVertex_p sv_tan0 = c0->E()->pData->getTangentSV(c0);
    if (sv_tan0==0) return; //assertion error

    if (vprev){
       sv_tan0->setPair(vprev->prev()->E()->pData->getTangentSV(vprev));
       Vec2 tan = P0(c0->next()) - P0(vprev->prev());
       sv_tan0->setTangent(tan/6.0,c0->E()->isBorder(), true);
    }else if(!vnext){
        sv_tan0->setPair(c0->prev()->E()->pData->getTangentSV(c0));
        Vec2 tan = P0(c0->next()) - P0(c0->prev());
        sv_tan0->setTangent(tan/6.0, c0->E()->isBorder(), true);
    }
}

void MeshShape::makeSmoothCorners(Corner_p pC, bool isskipsharp, int tangenttype, double tan_k){

    if(!pC)
        return;

    Corner_p c0 = pC;

    Corner_p vprev = c0->vPrev();
    Corner_p vnext = c0->vNext();

    ShapeVertex_p sv_tan0 = 0;
    ShapeVertex_p sv_tan1 = 0;
    Point p0, p1;

    if (vprev && vnext){

        sv_tan0 = c0->E()->pData->getTangentSV(c0);

        int val = 2;
        Corner_p c1 = vnext;
        while(vprev!=vnext){
            vnext = vnext->vNext();
            val++;
        }
        int skip = val/2 - 1 + (val%2);
        for(int i=0; i<skip; i++)
            c1 = c1->vNext();

        sv_tan1 = c1->E()->pData->getTangentSV(c1);

        p1 = P0(c1->next());
        p0 = P0(c0->next());

        //return; //this is not a corner
    }else if (!vprev && !vnext ){

        sv_tan1 = c0->E()->pData->getTangentSV(c0);
        sv_tan0 = c0->prev()->E()->pData->getTangentSV(c0);

        p1 = P0(c0->next());
        p0 = P0(c0->prev());

    }else if (vprev){

        sv_tan1 = c0->E()->pData->getTangentSV(c0);
        while(Corner_p c = vprev->vPrev()){
            vprev = c;
        }

        sv_tan0 = vprev->prev()->E()->pData->getTangentSV(vprev);

        //if (isskipsharp && (sv_tan0->pair() != c0->prev()->E()->pData->getTangentSV(c0))) return; //the border is sharp already

        p1 = P0(c0->next());
        p0 =  P0(vprev->prev());


    }else{

        sv_tan0 = c0->prev()->E()->pData->getTangentSV(c0);
        while(Corner_p c = vnext->vNext()){
            vnext = c;
        }

        sv_tan1 = vnext->E()->pData->getTangentSV(vnext);

        //if (isskipsharp && (sv_tan0->pair()!= c0->E()->pData->getTangentSV(c0))) return; //the border is sharp already

        p0 = P0(c0->prev());
        p1 = P0(vnext->next());
    }

    if (!isskipsharp && (sv_tan0->pair() || sv_tan1->pair()))
        return;

    sv_tan1->setPair(sv_tan0);
    if (tangenttype!=0){       
        Vec2 tan  = p1 - p0;
        Point p = P0(pC);

        if (tangenttype == 1)
            sv_tan1->setTangent(tan/6.0*tan_k,((Edge_p)(sv_tan1->pRef))->isBorder(), true);//c0->E()->isBorder()
        else{
            tan = tan.normalize();
            double a = (p-p0)*tan;
            double b = (p-p1)*tan;
            sv_tan0->setTangent(-tan*a/3.0*tan_k, false, false);
            sv_tan1->setTangent(-tan*b/3.0*tan_k, ((Edge_p)(sv_tan1->pRef))->isBorder(), false);
        }
    }
}

Bezier* initCurve(Edge_p e){

    if (!e->pData)
        e->pData = new EdgeData(e);

    if (e->pData->pCurve)
        return e->pData->pCurve;

    Vertex_p v0 = e->C0()->V();
    Vertex_p v1 = e->C0()->next()->V();
    Point p0 = v0->pData->P;
    Point p1 = v1->pData->P;

    Vec2 tan0 = p1 - p0;
    Vec2 tan1 = p0 - p1;

    MeshShape* shape = (MeshShape*) e->mesh()->caller();

    ShapeVertex_p sv0_t = shape->addVertex(p0 + tan0/3.0, v0->pData, true, false);
    ShapeVertex_p sv1_t = shape->addVertex(p1 + tan1/3.0, v1->pData, true, false);

    sv0_t->pRef = e;
    sv1_t->pRef = e;
    e->pData->pSV[1] = sv0_t;
    e->pData->pSV[2] = sv1_t;

    e->pData->pSV[0] = e->C0()->V()->pData;
    e->pData->pSV[3] = e->C0()->next()->V()->pData;

    Bezier* c = new Bezier(100);
    e->pData->pCurve = c;

    c->insert(v0->pData->pP());
    c->insert(sv0_t->pP());
    c->insert(sv1_t->pP());
    c->insert(v1->pData->pP());
    c->pRef = (void*) e;
    return c;
}

void onInsertEdge(Edge_p e){
    initCurve(e);
}

void onAddFace(Face_p pF)
{
    pF->pData->pSurface = (Patch*)new Patch4(pF);
}

void MeshShape::onSplitEdge(Corner_p c, double t)
{
    bool isforward = true;
    if (!c->isC0())
    {
        c = c->vNext();
        isforward = false;
    }

    MeshShape* pMS = (MeshShape*) c->V()->mesh()->caller();

    c->F()->Face::update(true);
    if (c->other())
        c->other()->F()->Face::update(true);

    //clean up
    Edge_p e0 = c->prev()->E();
    ShapeVertex_p svtan = e0->pData->pSV[2];
    c->V()->pData->adopt(e0->pData->pSV[2]);
    if (svtan->pair()){
        svtan->pair()->setPair(c->E()->pData->pSV[2]);
    }

    //parenting fixed
    //now adjust control p's
    Bezier* curve0 = e0->pData->pCurve;

    Point newCP[7];
    //bool isforward = curve0->pCV(0) == e0->C0()->V()->pData->pP();
    curve0->calculateDivideCV(isforward? t : (1-t), newCP);

    curve0->set(c->V()->pData->pP(), 3);

    curve0->pCV(1)->set(newCP[1]);
    curve0->pCV(2)->set(newCP[2]);
    curve0->pCV(3)->set(newCP[3]);

    Bezier* curve1 = c->E()->pData->pCurve;

    curve1->pCV(1)->set(newCP[4]);
    curve1->pCV(2)->set(newCP[5]);


}
