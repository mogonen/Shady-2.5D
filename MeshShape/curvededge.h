#ifndef CURVEDEDGE_H
#define CURVEDEDGE_H

#include "../shape.h"
#include "cmesh.h"


using namespace dlfl;

inline Point P0(Corner_p pC) {return pC->V()->pData->P();}
inline Point P1(Corner_p pC) {return pC->next()->V()->pData->P();}
inline Point P0(Edge_p pE)   {return pE->C0()->V()->pData->P();}
inline Point P1(Edge_p pE)   {return pE->C0()->next()->V()->pData->P();}

class CurvedEdge:public Selectable{

    int             _size;
    Point_p         _pts;

    inline int I(int i) const {CLAMP(i, 0, _size);}

    Edge_p          _pE;
    ShapeVertex_p   _pTanSV[2];

protected:

    //void onUpdate();

public:

    CurvedEdge(Edge_p, int sz = 0);
    ~CurvedEdge();

    Edge_p edge()const {return _pE;}

    void init(ShapeVertex_p sv0=0, ShapeVertex_p sv1=0);
    void set(Edge_p);
    void discard();
    void setRef(Referable_p pRef);

    ShapeVertex_p getTangentSV(int i) const {return _pTanSV[i];}
    ShapeVertex_p getTangentSV(ShapeVertex_p sv);
    ShapeVertex_p getTangentSV(Corner_p pC);

    Corner_p      getCornerByTan(ShapeVertex_p pC);

    double        computeDistance(const Point&, double &);
    void          computeSubdivisionCV(double t, Point[]);
    void          render(int mode = 0);

    Point CV(int i);
    Point_p pCV(int i);

    Point P(int i);
    int size();

};

#endif // CURVEDEDGE_H
