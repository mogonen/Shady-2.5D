#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "cmesh.h"
#include "curvededge.h"

#define N_MIN_Z 0.35

using namespace dlfl;

class Patch:public Selectable {

protected:
   Point* _ps;
   Vec3*  _ns;
   Face_p _pFace;
   Point  _K[16]; //bezier surface

   Point   K(int ei, int i);

   static  Vec3 decompose(const Vec3& v, const Vec3& nx);
   static  Vec3 compose(const Vec3& v, const Vec3& nx);

   int     _sampleU, _sampleV, _sampleUi, _sampleVi, _sampleUV;
   double  _Tu, _Tv;

public:


    Patch(Face_p);
    Corner* C(int i) const {return _pFace->C(i);}

    //virtual void interpolateNormals()=0;
    void propateNormals();
    Normal computeN(Corner_p);
    Normal computeN(Corner_p, double);

    int edgeInd(int ei, int i);
    int edgeUInd(int ei, int i);
    Point P(int i)const {return _ps[i];}

    int edgeI(int i,int j);
    int cornerI(int i, int j);

    static bool isH;
    inline int ind(int i, int j){return i + j*_sampleU;}

public:

    /*static void setN(int n){
        N = n;
        Ni = N-1;
        NN = n*n;
        NN2 = (NN + N) / 2;
        T = 1.0/Ni;
    }*/

    void setSample(int u, int v){
        u = (u<2)?2:u;
        v = (v<2)?2:v;
       _sampleU = u;
       _sampleV = v;
       _sampleUi = u-1;
       _sampleVi = v-1;
       _sampleUV = u*v;
       _Tu = 1.0 / _sampleUi;
       _Tv = 1.0 / _sampleVi;
    }

    int USamples(){return _sampleU;}
    int VSamples(){return _sampleV;}

    Face_p face() const {return _pFace;}

    static void flipH(){
        isH=!isH;
    }

};

class Patch4:public Patch{

    inline Point P(int i, int j)const{return _ps[i + j*_sampleU];}
    Vec3   interpolateN(int, int);

protected:

    void onUpdate();

public:

    virtual void render(int mode = 0);

    Patch4(Face_p);
    //~Patch4();

    void interpolateNormals();

};

class PatternPatch:public Patch{


protected:

    int*   _pattern;
    int    _nU, _nV;

public:

    virtual void assignPattern(int uv, int off, int len, int * data);
    virtual void init(int nu, int nv)=0;


    int U() const {return _nU;}
    int V() const {return _nV;}
    int UV(int uv = 0) const {return (uv==0) ? _nU:_nV;}

    PatternPatch(Face_p pF):Patch(pF){ _pattern = 0;}

    static int NU, NV;
};

class GridPattern:public PatternPatch{



protected:

    void onUpdate();

public:

    GridPattern(Face_p pF);
    void render(int mode = 0);
    void init(int nu, int nv);

    int getPattern(int i, int j) const;

    inline Point P(int i, int j)const {return _ps[i + j*_sampleU];}
};

class UVPatternPatch:public PatternPatch{

    int    _nU, _nV;

protected:

    void onUpdate();

public:

    void render(int mode = 0);
    void init(int nu, int nv);

    inline int ind(int uv, int n, int i, int j=0) const {return i + (j*_sampleU) + uv*(_nU*_sampleU)*2 + n*_sampleU*2;}
    inline Point P(int uv, int n, int i, int j=0) const {return _ps[i + (j*_sampleU) + uv*(_nU*_sampleU)*2 + n*_sampleU*2];}
    inline Point Pu(int n, int i, int j=0) const {return P(0,n,i,j);}
    inline Point Pv(int n, int i, int j=0) const {return P(1,n,i,j);}

    UVPatternPatch(Face_p);
    //~Patch4();

     static int NU, NV;
};


static double H(double t)
{
    return 3*t*t - 2*t*t;
}

class Rectangle:public Patch{


public:

    Rectangle(Face_p pF):Patch(pF){}
    void render(int mode = 0);
    void interpolateNormals(){}
};

#endif // PATCH_H
