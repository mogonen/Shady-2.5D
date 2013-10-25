#include "curve.h"
#define EPS 0.01

Curve::Curve(int sz):Selectable(Renderable::SHAPE){
    _isFlipped = _isClosed = false;
    _size = sz;
    _pts = new Point[sz];
}

Curve::~Curve(){
    delete [] _pts;
}

void Curve::onUpdate(){
    resample();
}

void Curve::resize(int newsize){
    if (_size!=newsize){
        delete [] _pts;
        _pts = new Point[newsize];
    }
    _size = newsize;
    update();
}

void Curve::resample(){
    for(int i=0; i<_size; i++)
        _pts[i] = evalP( i*1.0 / (_size-1) );
}

Vec2 Curve::evalT(double t) const{

    double t0 = t - EPS;
    double t1 = t + EPS;
    if (_isClosed){
        t1 = t1 > 1.0 ?(t1 -1.0) : t1;
        t0 = t0 < 0 ?  (t0 + 1.0): t0;
    }else{
        t1 = CLAMP(t1, 0, 1.0);
        t0 = CLAMP(t0, 0, 1.0);
    }

    return (evalP(t1) - evalP(t0)).normalize();
}

Vec2 Curve::evalN(double t) const{
    return ( Vec3(evalT(t)) % Vec3(0,0,1) ).normalize();
}

Vec2 Curve::N(int i) const{
    return ( Vec3(T(i)) % Vec3(0,0,1) ).normalize();
}

//returns the closest t where C(t)-p is min
double Curve::computeDistance(const Point& p, double &t){

    double mindist = 9999999;

    Point p0 = _pts[0];
    for(int i=1; i < _size; i++){

        Point p1 = _pts[i];

        double a = (p-p0)*(p1-p0).normalize();
        double h = sqrt( (p-p0).normsqr() - a*a);

        if (h < mindist){
            t =   ( i - 1 + (a / (p1-p0).norm()) )/(_size-1.0);// +  (a / (p1-p0).norm()) * (1/(_size-1.0));
            mindist = h;
        }

        p0 = p1;
    }

    return mindist;
}


Point Bezier::evalP(double t) const{
    Point p;
    FOR_ALL_I(4) p = p + cubicBernstein(i, t)*CV(i);
    return p;
}

void Bezier::calculateDivideCV(double t, Point newCV[])
 {
    newCV[0] = CV(0);
    newCV[6] = CV(3);

    newCV[1] = CV(0)*(1.0-t) + CV(1)*t;
    newCV[5] = CV(2)*(1.0-t) + CV(3)*t;


    Point p_12 = (CV(1)*(1-t)+CV(2)*t);

    newCV[2] = newCV[1]*(1-t) + p_12*t;
    newCV[4] = p_12*(1-t) + newCV[5]*t;

    newCV[3] = newCV[2]*(1-t) + newCV[4]*t;
 }
