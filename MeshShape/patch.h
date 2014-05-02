#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "cmesh.h"
#include "curvededge.h"

using namespace dlfl;

class Patch:public Selectable
{

protected:

    Face_p          _pFace;
    ShapeVec_p      _map;

    int             _sampleU, _sampleV, _sampleUi, _sampleVi, _sampleUV;
    double          _Tu, _Tv;


    Point           KVal(int ei, int i);
    Point           P(int i, int j);

    void            updateBezierPatch();
    void            onUpdate();

public:

    Patch(Face_p);
    Patch(int u, int v);
    ~Patch();

    virtual void    render(int mode = 0);

    Corner*         C(int i) const;
    int             edgeInd(int ei, int i) const;
    int             edgeUInd(int ei, int i) const;
    int             edgeI(int i,int j);
    int             cornerI(int i, int j);
    ShapeVec        CVec(int i);
    ShapeVec        get(int, int);

    Point           P(int i) const {return _map[i]._P;}
    Point           edgeP(int ei, int i) const;
    inline int      ind(int i, int j){return (i-1) + (j-1)*_sampleUi;}

    Face_p          face() const {return _pFace;}
    void            setSample(int u, int v);
    int             USamples(){return _sampleU;}
    int             VSamples(){return _sampleV;}

    void            computeBezierPatch(Point K[]);
    void            interpolateMap();

};

static double H(double t)
{
    return 3*t*t - 2*t*t;
}



#endif // PATCH_H
