#include "curvededge.h"
#include "Patch.h"

CurvedEdge::CurvedEdge(Edge_p pE, int sz):Selectable(false)
{
    _size = sz; // change later
    if (sz)
        _pts = new Point[_size];
    else
        _pts = 0;
    _pE = pE;
    _pTanSV[0] = _pTanSV[1] =0;
    setRef(_pE);
}

CurvedEdge::~CurvedEdge(){
    if (_pts)
    delete [] _pts;
}

void  CurvedEdge::set(Edge_p pE){
    _pE = pE;
    _pE->pData = this;
    if (_pTanSV[0]){
        _pTanSV[0]->setRef(_pE);
        _pTanSV[0]->_isDeleted = false;
    }

    if (_pTanSV[1]){
        _pTanSV[1]->setRef(_pE);
        _pTanSV[1]->_isDeleted = false; //markUndeleted();
    }
}

void CurvedEdge::discard(){
    if (_pTanSV[0])
        _pTanSV[0]->_isDeleted = true;

    if (_pTanSV[1])
        _pTanSV[1]->_isDeleted = true;
}

static const int FTABLE[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600};
/*
void CurvedEdge::onUpdate(){
    if (!_pts)
        return;
    Point CV[4] = {_pE->C0()->V()->pData->P(), _pTanSV[0]->P(), _pTanSV[1]->P(),_pE->C0()->next()->V()->pData->P()};
    for(int j=0; j<_size; j++){
        Point p;
        double t = j*1.0 / (_size-1);
        FOR_ALL_I(4) p = p + (6.0 / (FTABLE[3-i] * FTABLE[i]) * pow(1-t, 3-i) * pow(t,i))*CV[i];
        _pts[j] = p;
    }
}*/

void CurvedEdge::setRef(Referable_p pRef){
    if (ref()){
        CurvedEdge* ce = ((Edge_p)ref())->pData;
        if (ce)
            ce->discard();
    }
    set((Edge_p)pRef);
    Referrer::setRef(pRef);

}

void CurvedEdge::init(ShapeVertex_p sv0, ShapeVertex_p sv1){

    Shape_p shape = (Shape*) _pE->mesh()->caller();
    if (!sv0) sv0 = shape->addVertex((P0(_pE)*0.66 + P1(_pE)*0.34), _pE->C0()->V()->pData, true, false);
    if (!sv1) sv1 = shape->addVertex((P0(_pE)*0.34 + P1(_pE)*0.66), _pE->C1()->V()->pData, true, false);

    _pTanSV[0] = sv0;
    _pTanSV[1] = sv1;

    _pTanSV[0]->setRef(_pE);
    _pTanSV[1]->setRef(_pE);
}

Point CurvedEdge::CV(int i){
    return *pCV(i);
}

Point_p CurvedEdge::pCV(int i){
    if (i==0){
        return _pE->C0()->V()->pData->pP();
    }else if (i==1 || i==2){
        return _pTanSV[i-1]->pP();
    } else if (i==3){
        return _pE->C0()->next()->V()->pData->pP();
    }
    return 0;
}

Point CurvedEdge::P(int i){
    if (_pts){
        return _pts[i];
    }else{
        Corner_p  c = _pE->C();
        return c->F()->pData->P(c->F()->pData->edgeInd(c->I(), i));
    }
}

int CurvedEdge::size(){
    return _size?_size:(_pE->C()->I()?_pE->C()->F()->pData->VSamples() : _pE->C()->F()->pData->USamples() );
}

/*
Vec2 CurvedEdge::evalT(double t) const{

    double t0 = t - EPS;
    double t1 = t + EPS;
    if (_isClosed){
        t1 = t1 > 1.0 ?(t1 -1.0) : t1;
        t0 = t0 < 0 ?  (t0 + 1.0): t0;
    }else{
        t1 = CLAMP(t1, 0, 1.0);
        t0 = CLAMP(t0, 0, 1.0);
    }

    return Point();// (evalP(t1) - evalP(t0)).normalize();
}
*/

ShapeVertex_p CurvedEdge::getTangentSV(ShapeVertex_p sv)
{
    return (_pTanSV[0]->parent() == sv)? _pTanSV[0] : ((_pTanSV[1]->parent() == sv)? _pTanSV[1]:0);
}

ShapeVertex_p CurvedEdge::getTangentSV(Corner_p pC){
    return pC ? getTangentSV(pC->V()->pData) : 0;
}

void CurvedEdge::computeSubdivisionCV(double t, Point newCV[])
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

double CurvedEdge::computeDistance(const Point& p, double &t){

    double mindist = 9999999;

    Point p0 = P(0);
    int sz = size();
    for(int i=1; i < sz; i++){

        Point p1 = P(i);

        double a = (p-p0)*(p1-p0).normalize();
        double h = sqrt( (p-p0).normsqr() - a*a);

        if (h < mindist){
            t =   ( i - 1 + (a / (p1-p0).norm()) )/(sz-1.0);// +  (a / (p1-p0).norm()) * (1/(_size-1.0));
            mindist = h;
        }

        p0 = p1;
    }

    return mindist;
}


/*
Corner_p getCornerByTangent(ShapeVertex_p sv, bool isnext=false){

    if (!pCurve)
        return 0;

    if (pSV[1] == sv){
        return pE->C0();
    }else if (pSV[2]->parent() == sv){
        return  (pE->C1() || !isnext)? pE->C1():pE->C0()->next();
    }
}

static  Corner_p StaticGetCornerByTangent(ShapeVertex_p sv, bool isnext=false){
    Edge_p e = dynamic_cast<Edge_p>((Edge_p)sv->ref());
    if (!e || !e->pData || !e->pData->pCurve)
        return 0;
    return e->pData->getCornerByTangent(sv, isnext);
}
*/
