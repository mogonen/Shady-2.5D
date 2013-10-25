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

   EdgeData(Edge_p pe){
       pE = pe;
       pCurve = 0;
   }

   ShapeVertex_p getTangentSV(Corner_p pC){

       if (!pE->pData->pCurve)
           return 0;

       bool isForward = (pE->pData->pCurve->pCV(0) == pE->C0()->V()->pData->pP());
       if (pE->C0() == pC){
           return pSV[isForward?1:2];
       }else if (pE->C0()->next() == pC){
           return pSV[isForward?2:1];
       }
   }
};

struct FaceData{

   Patch* pSurface;

};

inline Point P0(Corner_p pC) {return pC->V()->pData->P;}
inline Point P0(Edge_p pE)  {return pE->C0()->V()->pData->P;}
inline Point P1(Edge_p pE)  {return pE->C0()->next()->V()->pData->P;}

#endif // MESHDATA_H
