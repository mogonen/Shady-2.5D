#ifndef PATTERN_H
#define PATTERN_H

#include "Patch.h"

class PatternPatch:public Patch{

protected:

    int*   _pattern;
    int    _nU, _nV;

public:

    virtual void assignPattern(int uv, int off, int len, int * data);
    virtual void init(int nu, int nv)=0;

    int     U() const {return _nU;}
    int     V() const {return _nV;}
    int     UV(int uv = 0) const {return (uv==0) ? _nU:_nV;}

    PatternPatch(Face_p pF):Patch(pF){ _pattern = 0;}
    static  int NU, NV;

};

class GridPattern:public PatternPatch
{

    Point*  _ps;

protected:
    void    onUpdate();

public:

    GridPattern(Face_p pF);
    void    render(int mode = 0);
    void    init(int nu, int nv);

    int     getPattern(int i, int j) const;

    inline  Point P(int i, int j) {return _ps[i+j*_sampleU];}
};

class UVPatternPatch:public PatternPatch
{
    Point*  _ps;
    int     _W;

protected:

    void onUpdate();

public:

    void    render(int mode = 0);
    void    init(int nu, int nv);

    int     getPattern(int i, int uv=0) const;

    inline  int ind(int uv, int n, int i, int j=0) const {return i + (j*_sampleU) + uv*(_nU*_sampleU)*_W + n*_sampleU*_W;}
    inline  Point P(int uv, int n, int i, int j=0) const {return _ps[i + (j*_sampleU) + uv*(_nU*_sampleU)*_W + n*_sampleU*_W];}
    inline  Point Pu(int n, int i, int j=0) const {return P(0,n,i,j);}
    inline  Point Pv(int n, int i, int j=0) const {return P(1,n,i,j);}

    UVPatternPatch(Face_p);
    //~Patch4();

    static int      NU, NV;
    static bool     isTHICK;
};


#endif // PATTERN_H
