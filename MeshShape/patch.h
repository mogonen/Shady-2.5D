#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "cmesh.h"
#include "curvededge.h"

#define N_MIN_Z 0.35

using namespace dlfl;

typedef RGB*                Map;
typedef std::vector<Map>    MapList;

class Patch:public Selectable
{

protected:

   Point*           _ps;
   MapList          _maps;
   Face_p           _pFace;

   int             _sampleU, _sampleV, _sampleUi, _sampleVi, _sampleUV;
   double          _Tu, _Tv;

   static Vec3     decompose(const Vec3& v, const Vec3& nx);
   static Vec3     compose(const Vec3& v, const Vec3& nx);

public:


    Patch(Face_p);

    Corner*         C(int i) const {return _pFace->C(i);}

    Normal          computeN(Corner_p);
    Normal          computeN(Corner_p, double);
    void            propateNormals(Point_p);
    void            propateMap(int);

    Point           P(int i)const {return _ps[i];}
    Face_p face()   const {return _pFace;}

    virtual int     edgeInd(int ei, int i){return -1;}
    virtual int     edgeUInd(int ei, int i){return -1;}

    void            setSample(int u, int v);
    int             USamples(){return _sampleU;}
    int             VSamples(){return _sampleV;}

};

class Patch4:public Patch
{

    inline Point P(int i, int j)const{ return _ps[i + j*_sampleU];}

protected:

    Point           KVal(int ei, int i);

    static RGB      interpolateCoonz(int i, int j, Map map, int U); //coonz
    void            interpolateMap(int channel);
    void            onUpdate();

public:

    Patch4(Face_p);

    virtual void    render(int mode = 0);

    int             edgeInd(int ei, int i);
    int             edgeUInd(int ei, int i);
    int             edgeI(int i,int j);
    int             cornerI(int i, int j);
    inline int      ind(int i, int j){return i + j*_sampleU;}

    static void     computeBezierPatch(Point *ps, Point K[], int U, int V);

};

//for non quad faces
class PatchN:public Patch{

    Point _K[16];
protected:
    void    onUpdate();

public:

    void    render(int mode = 0);
    PatchN(Face_p);

    Point   P(int n, int i, int j) {return _ps[i + j*_sampleU + n*_sampleUV];}

};

class PatternPatch:public Patch4{

protected:

    int*   _pattern;
    int    _nU, _nV;

public:

    virtual void assignPattern(int uv, int off, int len, int * data);
    virtual void init(int nu, int nv)=0;

    int     U() const {return _nU;}
    int     V() const {return _nV;}
    int     UV(int uv = 0) const {return (uv==0) ? _nU:_nV;}

    PatternPatch(Face_p pF):Patch4(pF){ _pattern = 0;}
    static  int NU, NV;

};

class GridPattern:public PatternPatch
{

protected:
    void    onUpdate();

public:

    GridPattern(Face_p pF);
    void    render(int mode = 0);
    void    init(int nu, int nv);

    int     getPattern(int i, int j) const;

    inline  Point P(int i, int j)const {return _ps[i + j*_sampleU];}
};

class UVPatternPatch:public PatternPatch
{

    int    _nU, _nV;

protected:

    void onUpdate();

public:

    void    render(int mode = 0);
    void    init(int nu, int nv);

    inline  int ind(int uv, int n, int i, int j=0) const {return i + (j*_sampleU) + uv*(_nU*_sampleU)*2 + n*_sampleU*2;}
    inline  Point P(int uv, int n, int i, int j=0) const {return _ps[i + (j*_sampleU) + uv*(_nU*_sampleU)*2 + n*_sampleU*2];}
    inline  Point Pu(int n, int i, int j=0) const {return P(0,n,i,j);}
    inline  Point Pv(int n, int i, int j=0) const {return P(1,n,i,j);}

    UVPatternPatch(Face_p);
    //~Patch4();

     static int NU, NV;
};


static double H(double t)
{
    return 3*t*t - 2*t*t;
}

class Rectangle:public Patch
{

public:

    Rectangle(Face_p pF):Patch(pF){}
    void render(int mode = 0);
    void interpolateNormals(){}
};

#endif // PATCH_H
