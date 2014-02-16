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

   void relink(Edge_p e){

       e->pData = this;

       pSV[0]->pRef = (void*) e->C0()->V();
       pSV[3]->pRef = (void*) e->C0()->next()->V();
       pSV[1]->pRef = (void*) e;
       pSV[2]->pRef = (void*) e;

       pE = e;
   }

   ShapeVertex_p getTangentSV(Corner_p pC)
   {

       if (!pE->pData || !pE->pData->pCurve)
           return 0;

       if ((pE->C0() == pC ) || ( pE->C1() && pE->C1()->next() == pC))
       {
           return pSV[1];
       }
       else if ( (pE->C1() == pC) || (pE->C0()->next() == pC))
       {
           return pSV[2];
       }
       return 0;
   }

   Corner_p getCornerByTangent(ShapeVertex_p sv, bool isnext=false){

       if (!pE->pData->pCurve)
           return 0;

       if (pSV[1] == sv){
           return pE->C0();
       }else if (pSV[2] == sv){
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

inline Point P0(Corner_p pC) {return pC->V()->pData->P;}
inline Point P0(Edge_p pE)   {return pE->C0()->V()->pData->P;}
inline Point P1(Edge_p pE)   {return pE->C0()->next()->V()->pData->P;}

#endif // MESHDATA_H
