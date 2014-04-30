#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "cmesh.h"
#include "curvededge.h"

#define N_MIN_Z 0.35

using namespace dlfl;

typedef RGB*                Map;
typedef std::vector<Map>    MapList;

class Surface:public Selectable
{

protected:

   Point*           _ps;
   MapList          _maps;
   Face_p           _pFace;

   int              _sampleU, _sampleV, _sampleUi, _sampleVi, _sampleUV;
   double           _Tu, _Tv;

public:

    Surface(Face_p);

    //virtual Corner* C(int i) const;

    virtual Point   P(int i) const {return _ps[i];}
    Face_p face()   const {return _pFace;}

    virtual Point  edgeP(int ei, int i) const {}

    virtual int     edgeInd(int ei, int i){return -1;}
    virtual int     edgeUInd(int ei, int i){return -1;}

    void            setSample(int u, int v);
    int             USamples(){return _sampleU;}
    int             VSamples(){return _sampleV;}

};

class Patch4:public Surface
{

    inline Point P(int i, int j)const{ return _ps[i + j*_sampleU];}

protected:

    Point           KVal(int ei, int i);

    static RGB      interpolateCoonz(int i, int j, Map map, int U); //coonz
    void            updateBezierPatch();

    void            onUpdate();

public:

    Patch4(Face_p);
    Patch4(int u, int v);

    virtual void    render(int mode = 0);

    Corner*         C(int i) const;
    int             edgeInd(int ei, int i) const;
    int             edgeUInd(int ei, int i) const;
    int             edgeI(int i,int j);
    int             cornerI(int i, int j);
    RGB             mapValue(int channel, int ei, float t);

    Point           edgeP(int ei, int i) const;
    inline int      ind(int i, int j){return i + j*_sampleU;}



    void            computeBezierPatch(Point K[]);
    void            interpolateMap(int channel);
    void            propateNormals(Normal[], int size=4);
    void            propateMap(int, RGB[], int size = 4);


    static Vec3     decompose(const Vec3& v, const Vec3& nx);
    static Vec3     compose(const Vec3& v, const Vec3& nx);

    Normal          computeN(Corner_p);
    Normal          computeN(Corner_p, double);

};

static double H(double t)
{
    return 3*t*t - 2*t*t;
}

class Rectangle:public Surface
{

public:

    Rectangle(Face_p pF):Surface(pF){}
    void render(int mode = 0);
    void interpolateNormals(){}
};



#if 0 //DEPRICATED
typedef std::vector<Patch4*> Patch4List;
//for non quad faces
class PatchN:public Surface{

    Patch4List _patches;

protected:
    void    onUpdate();

public:

    void    render(int mode = 0);
    PatchN(Face_p);

    Point   P(int n, int i, int j) {return _ps[i + j*_sampleU + n*_sampleUV];}
    Point edgeP(int ei, int i) const;

};
#endif







#endif // PATCH_H
