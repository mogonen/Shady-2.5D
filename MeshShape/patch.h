#ifndef PATCH_H
#define PATCH_H

#include "../Base.h"
#include "CMesh.h"
#include "MeshData.h"

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

    virtual void interpolateNormals()=0;
    void propateNormals();
    Normal computeN(Corner_p);

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


};

class Patch4:public Patch{

    inline Point P(int i, int j)const{return _ps[i + j*N];}
    Vec3    interpolateN(int, int);

protected:

    void onUpdate();

public:

    void render() const;

    Patch4(Face_p);
    //~Patch4();

    void interpolateNormals();

};

#endif // PATCH_H
