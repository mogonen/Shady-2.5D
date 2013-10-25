#include "meshshape.h"
#include "../curve.h"
#include "Patch.h"
#include "MeshData.h"

void MeshShape::onUpdate(){

    EdgeList edges = _control->edges();
    FOR_ALL_CONST_ITEMS(EdgeList, edges){
        Edge_p e = (*it);

        if (e->isBorder()){

        }

        if (e->pData->pCurve)
            e->pData->pCurve->update();

    }

    FaceList faces = _control->faces();
    FOR_ALL_CONST_ITEMS(FaceList, faces){
        if ((*it)->pData->pSurface)
            (*it)->pData->pSurface->update();
    }

    Selectable_p sel = Selectable::getTheSelected();

    /*if (getController()->doesExist((ControlPoint_p)sel)){
       //this is a control point
        ControlPoint_p cp = (ControlPoint_p)sel;

        Corner_p corn = cp->pV ? cp->pV->C() : ((ControlPoint_p)cp->parent())->pV->C();
        Corner_p corn0 = corn;
        while(corn){

            corn->E()->curve->outdate();
            corn->prev()->E()->curve->outdate();

            corn->E()->curve->update();
            corn->prev()->E()->curve->update();
            corn = corn->vNext();
            if (corn == corn0)
                break
        }
    }*/
}

void MeshShape::makeSmoothTangents(){

    EdgeList edges = _control->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        Edge_p e = (*it);
        Corner_p vprev = e->C0()->vPrev();
        Corner_p vnext = e->C0()->vNext();

        ShapeVertex_p tan0 = e->pData->getTangentSV(e->C0());
        if (vprev){
           tan0->setPair(vprev->prev()->E()->pData->getTangentSV(vprev),e->isBorder(), e->isBorder());
        }else if(!vnext){
            tan0->setPair(e->C0()->prev()->E()->pData->getTangentSV(e->C0()), true, true);
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

    /*if (MeshShape::isSMOOTH && e->isBorder()){
        tan0 = p1 - (e->C0()->prev()->V()->pData->P);
        tan1 = p0 - (e->C0()->next()->next()->V()->pData->P);
    }*/

    MeshShape* shape = (MeshShape*) e->mesh()->caller();

    ShapeVertex_p sv0_t = shape->addVertex(p0 + tan0/3.0, v0->pData, true, false);
    ShapeVertex_p sv1_t = shape->addVertex(p1 + tan1/3.0, v1->pData, true, false);

    sv0_t->pRef = e;
    sv1_t->pRef = e;
    e->pData->pSV[1] = sv0_t;
    e->pData->pSV[2] = sv1_t;

    Bezier* c = new Bezier(100);
    e->pData->pCurve = c;

    c->insert(v0->pData->pP());
    c->insert(sv0_t->pP());
    c->insert(sv1_t->pP());
    c->insert(v1->pData->pP());

    /*
    if (MeshShape::isSMOOTH && e->isBorder()){

        Edge_p e0 = e->C0()->prev()->E();
        if ( e0 && e0->isBorder() && e0->pData->pCurve){
            sv0_t->setPair(e0->pData->getTangentSV(e->C0()));
            tan0 = tan0.normalize();
            e->C0()->V()->pData->N.set( Vec3(0,0,1)%Vec3(tan0.x, tan0.y, 0));
        }

        Edge_p e1 = e->C0()->next()->E();
        if ( e1 && e1->isBorder() && e1->pData->pCurve){
            sv1_t->setPair(e1->pData->getTangentSV(e->C0()->next()));
            tan1 = -tan1.normalize();
            e->C0()->next()->V()->pData->N.set( Vec3(0,0,1)%Vec3(tan1.x, tan1.y, 0));
        }
    }*/

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
    c = c->isC0()? c : c->vNext();

    MeshShape* pMS = (MeshShape*) c->V()->mesh()->caller();

    c->F()->Face::update(true);
    if (c->other())
        c->other()->F()->Face::update(true);

    Edge_p e0 = c->prev()->E();
    Bezier* curve0 = e0->pData->pCurve;

    bool isForward = (c->prev()->V()->pData->pP() == curve0->pCV(0));


    Point newCP[7];
    curve0->calculateDivideCV(t, newCP);
    c->V()->pData->adopt(e0->pData->pSV[isForward?2:1]);

    //parenting fixed
    //now adjust control p's

    curve0->set(c->V()->pData->pP(), isForward?3:0);

    curve0->pCV(isForward?1:2)->set(newCP[isForward?1:6]);
    curve0->pCV(isForward?2:1)->set(newCP[isForward?2:5]);
    curve0->pCV(isForward?3:0)->set(newCP[isForward?3:4]);

    Bezier* curve1 = c->E()->pData->pCurve;

    curve1->pCV(isForward?1:2)->set(newCP[isForward?4:3]);
    curve1->pCV(isForward?2:1)->set(newCP[isForward?5:2]);


    /*ControlNormal_p cpnorm = (ControlNormal_p)c->V()->pData->pCP->getChilds().front();
    if (cpnorm){
        cpnorm->pP()->set(c->V()->pData->P);
    }*/
}
