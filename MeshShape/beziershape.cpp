#include "meshshape.h"
#include "../curve.h"
#include "Patch.h"

#ifdef MODELING_MODE
#include "pattern.h"
#endif

void  MeshShape::outdate(ShapeVertex_p sv){

    if (sv->ref()->isDeleted())
        return;

    if (sv->parent()){ //if tagent
        Edge_p e = (Edge_p)sv->ref();
        if (!e || !e->pData)
            return;

        e->pData->outdate();
        Face_p f0 = e->C()->F();
        Face_p f1 = e->C()->other()->F();

        if (f0->pData)
            f0->pData->outdate();

        if (!e->isBorder() && f1->pData)
            f1->pData->outdate();

    }else{//this is vertex
        Vertex_p v = (Vertex_p)sv->ref();
        if (!v)
            return;
        Corner_p c = v->C();
        if (!c)
            return;
        Corner_p c0 = c;
        while(c){
            if (c->F() && c->F()->pData)
                c->F()->pData->outdate();
            if (c->E() && c->E()->pData)
                c->E()->pData->outdate();
            c = c->vNext();
            if (c==c0){
                break;
            }
        }

    }
}

void updateCurve(Edge_p pE){
    if (pE->pData)
        pE->pData->update();
}

void updateSurface(Face_p pF){
    if (pF->pData && !pF->isBorder())
        pF->pData->update();
}

void ensureUpToDateCurve(Edge_p pE){
    if (pE->pData)
        pE->pData->ensureUpToDate();
}

void ensureUpToDateSurface(Face_p pF){
    if (pF->pData && !pF->isBorder())
        pF->pData->ensureUpToDate();
}

void MeshShape::onUpdate(){
    assignDepthValues();
    _control->ForAllEdges(updateCurve);
    _control->ForAllFaces(updateSurface);
}

void MeshShape::onEnsureUpToDate(){
    _control->ForAllEdges(ensureUpToDateCurve);
    _control->ForAllFaces(ensureUpToDateSurface);
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

void MeshShape::makeSmoothCorners(Corner_p pC, bool isskipsharp, int tangenttype, double tan_k)
{

    if(!pC)
        return;

    Corner_p c0 = pC;

    Corner_p vprev = c0->vPrev();
    Corner_p vnext = c0->vNext();

    ShapeVertex_p sv_tan0 = 0;
    ShapeVertex_p sv_tan1 = 0;
    Point p0, p1;

    if (!vprev->isBorder() && !vnext->isBorder())
    {

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
    }
    else if (vprev->isBorder() && vnext->isBorder()){

        sv_tan1 = c0->E()->pData->getTangentSV(c0);
        sv_tan0 = c0->prev()->E()->pData->getTangentSV(c0);

        p1 = P0(c0->next());
        p0 = P0(c0->prev());

    }else if (vnext->isBorder()){

        sv_tan1 = c0->E()->pData->getTangentSV(c0);
        sv_tan0 = vnext->E()->pData->getTangentSV(vnext);

        //if (isskipsharp && (sv_tan0->pair() != c0->prev()->E()->pData->getTangentSV(c0))) return; //the border is sharp already

        p1 = P0(c0->next());
        p0 =  P0(vnext->next());


    }else{

        sv_tan0 = c0->prev()->E()->pData->getTangentSV(c0);
        sv_tan1 = vprev->prev()->E()->pData->getTangentSV(vprev);

        //if (isskipsharp && (sv_tan0->pair()!= c0->E()->pData->getTangentSV(c0))) return; //the border is sharp already

        p0 = P0(c0->prev());
        p1 = P0(vprev->prev());
    }

    if (!isskipsharp && (sv_tan0->pair() || sv_tan1->pair()))
        return;

    sv_tan1->setPair(sv_tan0);
    if (tangenttype!=0){
        Vec2 tan  = p1 - p0;
        Point p = P0(pC);

        if (tangenttype == 1)
            sv_tan1->setTangent(tan/6.0*tan_k,((Edge_p)(sv_tan1->ref()))->isBorder(), true);//c0->E()->isBorder()
        else{
            tan = tan.normalize();
            double a = (p-p0)*tan;
            double b = (p-p1)*tan;
            sv_tan0->setTangent(-tan*a/3.0*tan_k, false, false);
            sv_tan1->setTangent(-tan*b/3.0*tan_k, ((Edge_p)(sv_tan1->ref()))->isBorder(), false);
        }
    }
}


void onInsertEdge(Edge_p e){
    if (e->pData)
        e->pData->setRef(e);
    else
        initCurve(e->C0());
}

void onAddFace(Face_p pF)
{
    if (pF->isBorder())
        return;

    ((MeshShape*)pF->mesh()->caller())->addSurface(pF);
}

