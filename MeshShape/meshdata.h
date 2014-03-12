#ifndef MESHDATA_H
#define MESHDATA_H

#include "CMesh.h"
#include "../Shape.h"
#include "../Curve.h"

using namespace dlfl;
class Patch;

struct EdgeData
{

   Bezier* pCurve;
   ShapeVertex_p pSV[4];
   Edge_p  pE;

   EdgeData(Edge_p pe)
   {
       pE = pe;
       pCurve = 0;
   }

   void initCurve(ShapeVertex_p sv1, ShapeVertex_p sv2){

       if (!sv1 || !sv2){
           Line* pL = new Line();
           pCurve = pL;
           pL->insert(pE->C0()->V()->pData->pP());
           pL->insert(pE->C1()->V()->pData->pP());
           pL->pRef = (void*) pE;
       }else{
           pCurve = new Bezier(100);
           pSV[1] = sv1;
           pSV[2] = sv2;
       }
       relink(pE);
   }

   void relink(Edge_p pe){
       pE = pe;
       pE->pData = this;

       if (pCurve->count() ==2){
           pSV[0] = pE->C0()->V()->pData;
           pSV[0]->pRef = (void*)pE->C0()->V();
           pSV[1] = pE->C0()->next()->V()->pData;
           pSV[1]->pRef = (void*)pE->C0()->next()->V();
       }else{
           pSV[0] = pE->C0()->V()->pData;
           pSV[0]->pRef = (void*)pE->C0()->V();
           pSV[3] = pE->C0()->next()->V()->pData;
           pSV[3]->pRef = (void*)pE->C0()->next()->V();

           pSV[1]->pRef = (void*)pE;
           pSV[2]->pRef = (void*)pE;

           //reanimate vertices, these should be handled better way
           pSV[0]->_isDeleted = false;
           pSV[1]->_isDeleted = false;
           pSV[2]->_isDeleted = false;
           pSV[3]->_isDeleted = false;

           pCurve->set(pSV[0]->pP(),0);
           pCurve->set(pSV[1]->pP(),1);
           pCurve->set(pSV[2]->pP(),2);
           pCurve->set(pSV[3]->pP(),3);
       }
   }

   ShapeVertex_p getTangentSV(ShapeVertex_p sv)
   {
       return (pSV[1]->parent() == sv)? pSV[1] : ((pSV[2]->parent() == sv)? pSV[2]:0);
   }

   ShapeVertex_p getTangentSV(Corner_p pC){
       return pC ? getTangentSV(pC->V()->pData) : 0;
   }

   Corner_p getCornerByTangent(ShapeVertex_p sv, bool isnext=false){

       if (!pCurve)
           return 0;

       if (pSV[1] == sv){
           return pE->C0();
       }else if (pSV[2]->parent() == sv){
           return  (pE->C1() || !isnext)? pE->C1():pE->C0()->next();
       }
   }

   static  Corner_p StaticGetCornerByTangent(ShapeVertex_p sv, bool isnext=false){
       Edge_p e = dynamic_cast<Edge_p>((Edge_p)sv->pRef);
       if (!e || !e->pData || !e->pData->pCurve)
           return 0;
       return e->pData->getCornerByTangent(sv, isnext);
   }
};

struct FaceData
{
   Patch* pSurface;
};

inline Point P0(Corner_p pC) {return pC->V()->pData->P();}
inline Point P1(Corner_p pC) {return pC->next()->V()->pData->P();}
inline Point P0(Edge_p pE)   {return pE->C0()->V()->pData->P();}
inline Point P1(Edge_p pE)   {return pE->C0()->next()->V()->pData->P();}

#endif // MESHDATA_H
