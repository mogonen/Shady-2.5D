#ifndef ELLIPSESHAPE_H
#define ELLIPSESHAPE_H

#include "shape.h"

class EllipseShape:public Shape{

    //ShapeVertex_p           _pSVa;
    //ShapeVertex_p           _pSVb;

    int                     _segU, _segV;

    Point*                  _ps;
    Normal*                 _ns;

    double                  _radX, _radY;

    inline int ind(int i, int j) const{return (i%_segU) + (j%_segV)*_segU;}

protected:

    void onUpdate(){

        //double radX = _pSVa->P.norm();
        //double radY = _pSVb->P.norm();

        double step_u = 2*PI / _segU;
        double step_v = PI / (2*_segV);

        FOR_ALL_J(_segV){
            FOR_ALL_I(_segU){
                double ang_u = -step_u * i;
                double ang_v = step_v * j;
                Point p(cos(ang_u)*_radX, sin(ang_u)*_radY);
                _ps[i+j*_segU] = p*cos(ang_v);
                _ns[i+j*_segU] = Normal(cos(ang_u)*cos(ang_v), sin(ang_u)*cos(ang_v), sin(ang_v)).normalize();
            }
        }
    }

    void onApplyT(const Matrix3x3& tM){
        Vec3 v = tM*Vec3(_radX, _radY, 0);
        _radX = v.x;
        _radY = v.y;

        v = tM*Vec3(0, 0, 1);
        pP()->set(P() + Point(v.x/v.z, v.y/v.z));
    }

public:

    EllipseShape():Shape(){
        //_pSVa = addVertex(Point(Radius,0));
        //_pSVb = addVertex(Point(0, Radius));
        _radX = _radY = Radius;
        _segU = 64;
        _segV = 16;
        _ps = new Point[_segU*_segV];
        _ns = new Normal[_segU*_segV];
        Renderable::update();
    }

    ~EllipseShape(){
        delete _ps;
        delete _ns;
    }

    void render() const;

    void getBBox(BBox& bbox) const{
        bbox.P[0].set(_radX>0?-_radX:_radX, _radY>0?-_radY:_radY);
        bbox.P[1].set(_radX>0?_radX:-_radX, _radY>0?_radY:-_radY);
    }

    static double Radius;
};

#endif // ELLIPSESHAPE_H
