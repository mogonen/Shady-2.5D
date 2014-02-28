#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "CMesh.h"
#include "MeshData.h"

#define N_MIN_Z 0.35

using namespace dlfl;

class Patch:public Selectable {

protected:
   Point* _ps;
   Vec3*  _ns;
   Face_p _pFace;
   Point _K[16]; //bezier surface

   Point   K(int ei, int i);

   static Vec3 decompose(const Vec3& v, const Vec3& nx);
   static Vec3 compose(const Vec3& v, const Vec3& nx);

   static int edgeInd(int ei, int i);
   static int edgeUInd(int ei, int i);

public:

    Patch(Face_p);
    Corner* C(int i) const {return _pFace->C(i);}

    //virtual void interpolateNormals()=0;
    void propateNormals();
    Normal computeN(Corner_p);
    Normal computeN(Corner_p, double);


    static bool isH;

protected:
    static inline int ind(int i, int j){return i + j*N;}
    static int N, Ni, NN, NN2;
    static double T;

public:

    static void setN(int n){
        N = n;
        Ni = N-1;
        NN = n*n;
        NN2 = (NN + N) / 2;
        T = 1.0/Ni;
    }
    static void flipH(){
        isH=!isH;
    }

};

class Patch4:public Patch{

    inline Point P(int i, int j)const{return _ps[i + j*N];}
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




    int    _nU, _nV;
    int*   _pattern;

protected:

    void onUpdate();

public:

    void render(int mode = 0);
    void init(int nu, int nv);

    void assignPattern(int uv, int off, int len, int * data);

    int U() const {return _nU;}
    int V() const {return _nV;}
    int UV(int uv = 0) const {return (uv==0) ? _nU:_nV;}

    inline int ind(int uv, int n, int i, int j=0) const {return i + (j*N) + uv*(_nU*N)*2 + n*N*2;}
    inline Point P(int uv, int n, int i, int j=0) const {return _ps[i + (j*N) + uv*(_nU*N)*2 + n*N*2];}
    inline Point Pu(int n, int i, int j=0) const {return P(0,n,i,j);}
    inline Point Pv(int n, int i, int j=0) const {return P(1,n,i,j);}

    int getN(){return N;}

    PatternPatch(Face_p);
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
