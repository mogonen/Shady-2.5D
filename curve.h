#ifndef CURVE_H
#define CURVE_H

#include <vector>
#include "Base.h"

typedef std::vector<Point_p> Point_pVec;


class Curve: public Selectable {

    int _size;
    Point_p _pts;

    bool _isFlipped;
    bool _isClosed;

    inline int I(int i) const { return _isClosed ? ((i+_size)%_size)  : CLAMP(i, 0, _size);}

protected:

    void onUpdate();

public:

    Curve(int sz = 100);
    ~Curve();

    virtual Point evalP(double) const = 0;
    virtual Vec2 evalT(double) const;
    virtual Vec2 evalN(double) const;

    Point P(int i) const { return _pts[I(i)];}
    Vec2 T(int i) const { return (_pts[I(i+1)] - _pts[I(i-1)] ).normalize();}
    Vec2 N(int i) const;

    double computeDistance(const Point&, double &);


    void flip(){_isFlipped = !_isFlipped;}
    bool isClosed() const {return _isClosed;}

    void resize(int);
    virtual void resample();

    void render() const;

};

class Spline:public Curve {

    Point_pVec _cvs;

public:
    Spline(int sz):Curve(sz){}
    Point CV(int i) const{ return *_cvs[i];}
    Point_p pCV(int i)const { return _cvs[i];}
    void insert(Point_p p){ _cvs.push_back(p);}
    void set(Point_p pP, int i){_cvs[i] = pP;}
};

class Bezier:public Spline{

public:
    Bezier(int sz):Spline(sz){}
    Point evalP(double) const;

    void calculateDivideCV(double t, Point[]);
};

static const int FTABLE[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600};
inline double cubicBernstein(int i, double t){
    return 6.0 / (FTABLE[3-i] * FTABLE[i]) * pow(1-t, 3-i) * pow(t,i);
}

#endif // CURVE_H
