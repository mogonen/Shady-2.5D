#include "curvededge.h"
#include "Patch.h"
#define N_MIN_Z 0.35
#define SAMPLE_DIST 0.01

static const int FTABLE[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600};
inline double cubicBernstein(int i, double t)
{
    return 6.0 / (FTABLE[3-i] * FTABLE[i]) * pow(1-t, 3-i) * pow(t,i);
}

inline int lodToSize(int lod){return lod*4+1;}
inline int sizeToLod(int size){return (size-1)/4;}

CurvedEdge::CurvedEdge(Edge_p pE, int sz):Selectable(false)
{
    _map = 0;
    _pts = 0;
    _pE = pE;
    _pTanSV[0] = _pTanSV[1] = 0;
    setRef(_pE);
    _size = 0;
    //resize(11);
}

CurvedEdge::~CurvedEdge(){
    if (_pts)
        delete [] _pts;

    if (_map)
        delete _map;
}

void CurvedEdge::onOutdate(){
/*
    int maxlod =  traverseEdges(_pE->C());
    if (maxlod > sizeToLod(_size))
        traverseEdges(_pE->C(),maxlod);*/
}

void CurvedEdge::onUpdate(){

    if (_size<2)
    {
        //resize(11);
        int maxlod = traverseEdges(_pE->C());
        traverseEdges(_pE->C(), maxlod);
    }

    Point K[4]{CV(0),CV(1), CV(2), CV(3)};
    double T = 1.0 / (_size-1);

    ShapeVec v0 = *_pE->C0()->V()->pData;
    ShapeVec v1 = *_pE->C1()->V()->pData;
    v0.value[NORMAL_CHANNEL] = computeN(_pE->C0());
    v1.value[NORMAL_CHANNEL] = computeN(_pE->C1());

    for(int i=0; i<_size; i++){
        double t = T*i;
        _map[i] = v0*(1-t) + v1*t;

        //compute P
        Point p;
        FOR_ALL_J(4) p = p + cubicBernstein(j, T*i)*K[j];
        _map[i]._P = p;
    }

    //propogate normals
    Vec2 tan0 = (_map[1]._P - _map[0]._P);
    Vec2 tan1 = (_map[_size-1]._P - _map[_size-2]._P);

    Vec3 n0_d = CurvedEdge::decompose(_map[0].value[NORMAL_CHANNEL], Vec3(tan0));
    Vec3 n1_d = CurvedEdge::decompose(_map[_size-1].value[NORMAL_CHANNEL], Vec3(tan1));

    for(int j=1; j<_size-1; j++)
    {
        double t = j*(1.0/(_size-1));
        Vec3 n_d = n0_d*(1-t) + n1_d*(t);
        Vec3 tan = (_map[j+1]._P - _map[j-1]._P);
        _map[j].value[NORMAL_CHANNEL] = CurvedEdge::compose(n_d, tan);
    }
}

int CurvedEdge::traverseEdges(Corner_p pC, int lod){

    int maxlod = 0;
    Corner* c0 = pC;

    for(int i=0; i<2; i++){
        do{

            if (lod>-1){
                c0->E()->pData->resize(lodToSize(lod));
            }else{
                int lod_i = c0->E()->pData->computeLOD();
                if (lod_i >  maxlod) maxlod = lod_i;
                //int size_i = c0->E()->pData->idealSize();
                //if (maxsize<size_i) maxsize = size_i;
            }

            if (c0->isBorder())
                break;

            c0 = c0->prev()->vPrev();
        }while(c0!=pC);

        if (c0==pC)
            return maxlod;

        c0 = pC->other();
    }
    return maxlod;
}

void CurvedEdge::resize(int size){

    if (size==_size)
        return;

    if (_map)
        delete _map;

    _size = size;
    if (_size){
        _map = new ShapeVec[_size];
    }
    outdate(false);
    if (_pE->C0()->F()->pData)
        _pE->C0()->F()->pData->outdate();

    if (_pE->C1()->F()->pData)
        _pE->C1()->F()->pData->outdate();
}

int CurvedEdge::idealSize(){
    Point K[4]={CV(0),CV(1), CV(2), CV(3)};
    double a = ((Vec3(K[1]- K[0])%Vec3(K[2]- K[0])).norm() + (Vec3(K[1]- K[3])%Vec3(K[2]- K[3])).norm());
    int  size = a / (SAMPLE_DIST*SAMPLE_DIST) + 1;
    size = pow(size, 0.5);
    return size>2 ? size : 2;
}

int CurvedEdge::computeLOD(){
    return 2;
    Point K[4]={CV(0),CV(1), CV(2), CV(3)};
    //double a = ((Vec3(K[1]- K[0])%Vec3(K[2]- K[0])).norm() + (Vec3(K[1]- K[3])%Vec3(K[2]- K[3])).norm());
    double l = (CV(0) - CV(3)).norm();
    return (int) pow(l/SAMPLE_DIST, 0.5);
}

/*
void CurvedEdge::propagateSize(dlfl::Corner_p pC, int size){
    Corner* c0 = pC;
    do{
        if (c0->E()->pData->size()!= size)
            c0->E()->pData->resize(size);

        if (c0->isBorder())
            break;

        c0 = c0->prev()->vPrev();
    }while(c0 !=pC);
}
*/
void  CurvedEdge::set(Edge_p pE){
    _pE = pE;
    _pE->pData = this;
    if (_pTanSV[0]){
        _pTanSV[0]->setRef(_pE);
        _pTanSV[0]->_isDeleted = false;
        _pTanSV[0]->setPair(_pTanSV[0]->pair());
    }

    if (_pTanSV[1]){
        _pTanSV[1]->setRef(_pE);
        _pTanSV[1]->_isDeleted = false; //markUndeleted();
        _pTanSV[1]->setPair(_pTanSV[1]->pair());
    }
}

void CurvedEdge::discard(){
    if (_pTanSV[0])
        _pTanSV[0]->_isDeleted = true;

    if (_pTanSV[1])
        _pTanSV[1]->_isDeleted = true;
}

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
        return _map[i]._P;
    }
}

Corner_p CurvedEdge::getCornerByTan(ShapeVertex_p pSV){

    if (pSV == _pTanSV[0])
        return _pE->C0();

    if (pSV == _pTanSV[1])
        return _pE->C1();

    return 0;
}

ShapeVertex_p CurvedEdge::getTangentSV(ShapeVertex_p pSV)
{
    if (pSV == _pE->C0()->V()->pData)
        return _pTanSV[0];

    if (pSV == _pE->C1()->V()->pData)
        return _pTanSV[1];

    return 0;
}


Point CurvedEdge::evalP(double t){
    Point p;
    FOR_ALL_I(4) p = p + cubicBernstein(i, t)*CV(i);
    return p;
}


Normal CurvedEdge::computeN(Corner_p c)
{
    Vec3 n = c->V()->pData->N();
    return n.z>N_MIN_Z ? n.normalize() : Vec3(n.x, n.y, N_MIN_Z).normalize();
}

Vec3 CurvedEdge::decompose(const Vec3& v, const Vec3& nx)
{
    Vec3 nnx = nx.normalize();
    Vec3 nz(0, 0, 1);
    Vec3 ny = nz%nnx;
    return Vec3(v*nnx, v*ny, v*nz);
}

Vec3 CurvedEdge::compose(const Vec3& v, const Vec3& nx)
{
    Vec3 nnx = nx.normalize();
    Vec3 nz(0,0,1);
    Vec3 ny = nz%nnx;
    return nnx*v.x + ny*v.y + nz*v.z;
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
/*
ShapeVertex_p CurvedEdge::getTangentSV(ShapeVertex_p sv)
{
    return (_pTanSV[0]->parent() == sv)? _pTanSV[0] : ((_pTanSV[1]->parent() == sv)? _pTanSV[1]:0);
}*/

ShapeVertex_p CurvedEdge::getSV(int i) const{
    if (i==0)
        return  _pE->C0()->V()->pData;

    if (i==3)
        return  _pE->C1()->V()->pData;

    if (i==1 || i==2)
        return  _pTanSV[i-1];
}

ShapeVertex_p CurvedEdge::getTangentSV(Corner_p pC){
    return pC ? getTangentSV(pC->V()->pData) : 0;
}

void CurvedEdge::computeSubdivisionCV(double t, Point newCV[], ShapeVec &vec)
 {
    newCV[0] = CV(0);
    newCV[6] = CV(3);

    newCV[1] = CV(0)*(1.0-t) + CV(1)*t;
    newCV[5] = CV(2)*(1.0-t) + CV(3)*t;


    Point p_12 = (CV(1)*(1-t)+CV(2)*t);

    newCV[2] = newCV[1]*(1-t) + p_12*t;
    newCV[4] = p_12*(1-t) + newCV[5]*t;

    newCV[3] = newCV[2]*(1-t) + newCV[4]*t;

    float fi = _size*t;
    int i = (int)fi;
    float ti = fi-i;
    vec = _map[i]*(1-ti) + _map[i+1]*ti;
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
