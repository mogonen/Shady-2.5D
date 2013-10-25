#include "Patch.h"
#include "../curve.h"

int     Patch::N;
int     Patch::Ni;
int     Patch::NN;
int     Patch::NN2;
double  Patch::T;

Patch::Patch(Face_p pF):Selectable(Renderable::SHAPE){
    _ps = 0;
    _ns = 0;
    _pFace = pF;
    _pFace->pData = new FaceData();
    _pFace->pData->pSurface = this;
    this->pRef = (void*)pF;
}

void Patch::propateNormals(){

    Normal corner_ns[4];
    for(int i=0; i<_pFace->size(); i++)
        corner_ns[i] = computeN(C(i));

    for(int i=0; i<_pFace->size(); i++){

        Vec3 n0 = corner_ns[i];
        Vec3 n1 = corner_ns[(i+1)%4];
        _ns[edgeInd(i, 0)] = n0;
        _ns[edgeInd(i, Ni)] = n1;

        Vec2 tan0 = (_ps[edgeInd(i, 1)] - _ps[edgeInd(i, 0)]).normalize();
        Vec2 tan1 = (_ps[edgeInd(i, Ni)] - _ps[edgeInd(i, Ni-1)]).normalize();

        Vec3 n0_d = Patch::decompose(n0, Vec3(tan0));
        Vec3 n1_d = Patch::decompose(n1, Vec3(tan1));

        for(int j=1; j<N-1; j++){
            double t = j*T;
            Vec3 n_d = n0_d*(1-t) + n1_d*(t);
            Vec3 tan = (_ps[edgeInd(i, j+1)] - _ps[edgeInd(i, j-1)]).normalize();
            //compose
            _ns[edgeInd(i, j)] = compose(n_d, tan);
        }
    }
}

Patch4::Patch4(Face_p pF):Patch(pF){
    _ps = new   Point[NN];
    _ns = new   Vec3[NN];
}

Vec3 Patch4::interpolateN(int i, int j){
    double t = i*1.0 / Ni;
    double s = j*1.0 / Ni;
    Vec3 n0 = _ns[i]*(1.0-s) + _ns[i + Ni*N]*s;
    n0 = n0 + _ns[0 + j*N]*(1.0 - t) + _ns[Ni + j*N]*t;
    n0 = n0 - _ns[0]*(1-s)*(1.0-t) - _ns[Ni]*(1-s)*t - _ns[Ni + Ni*N]*s*t - _ns[Ni*N]*s*(1-t);
    return n0;
}

void Patch4::onUpdate(){

    _pFace->update();

    //init bezier surface points
    for(int i=0; i<4; i++){
        _K[i]= K(0,i);
        _K[i+12]= K(2,i);
    }
    _K[7] = K(1,1);
    _K[11] = K(1,2);

    _K[4] = K(3,1);
    _K[8] = K(3,2);

    _K[5]  = _K[1] + _K[4] - _K[0];
    _K[6] = _K[2] + _K[7] - _K[3];
    _K[9] = _K[13] + _K[8] - _K[12];
    _K[10] = _K[11] + _K[14] - _K[15];

    //bezier surface interpolation
    for(int j=0; j<N;j++)
        for(int i=0; i<N; i++){
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*T)*cubicBernstein(bj, j*T)*_K[bi+bj*4];
            _ps[ind(i,j)] = p;
        }

    propateNormals();
    interpolateNormals();
}

void Patch4::interpolateNormals(){
    for(int j = 1; j<Ni;j++)
        for(int i = 1; i<Ni; i++)
                _ns[ind(i,j)] = interpolateN(i, j);
}

int Patch::edgeInd(int ei, int i){
    switch(ei){
        case 0:
            return i;
        break;
        case 1:
            return Ni+i*N;
        break;
        case 2:
            return (Ni-i)+Ni*N;
        break;
        case 3:
            return (Ni-i)*N;
        break;
    }
    return -1;
}

int Patch::edgeUInd(int ei, int i){
    switch(ei){
        case 0:
            return i;
        break;
        case 1:
            return Ni+i*N;
        break;
        case 2:
            return i+Ni*N;
        break;
        case 3:
            return i*N;
        break;
    }
    return -1;
}

Vec3 Patch::decompose(const Vec3& v, const Vec3& nx){
    Vec3 nz(0, 0, 1);
    Vec3 ny = nz%nx;
    return Vec3(v*nx, v*ny, v*nz);
}

Vec3 Patch::compose(const Vec3& v, const Vec3& nx){
    Vec3 nz(0,0,1);
    Vec3 ny = nz%nx;
    return nx*v.x + ny*v.y + nz*v.z;
}

Point Patch::K(int ei, int i){
    Corner* ci = C(ei);
    Spline* c = ci->E()->pData->pCurve;
    /*if (!c)
        return (i<2)?(ci->P()*(1-i) + ci->next()->P()*i) : (ci->P()*(i-2) + ci->next()->P()*(1-t));*/

    bool curvedir = (ci->V()->pData->pP() == c->pCV(0));
    bool rev = ((ei>1) && curvedir) || ( (ei<2) && !curvedir);
    return (rev)?c->CV(3-i):c->CV(i);
}

Normal Patch::computeN(Corner_p c){
    /*Point p = P(c);
    Point p1 = c->E()->curve->CV(c->isC0()?1:2);					//next
    Point p0 = c->prev()->E()->curve->CV(c->prev()->isC0()?2:1);   //prev
    Point p2 = p0 + (p1-p);*/
    return c->V()->pData->N;
}

/*

Vec3 Patch4::cornerN(Corner* c){
    Vec3 p = c->P();
    Vec3 p1 = c->E()->curve->getP(c->isC0()?1:2);					//next
    Vec3 p0 = c->prev()->E()->curve->getP(c->prev()->isC0()?2:1);   //prev
    Vec3 p2 = p0 + (p1-p);
    return quadNormal2(p0, p, p1, p2);
}

Vec3 Patch4::cornerN2(Corner* c){
    Bezier* bc = (Bezier*)c->E()->curve;
    Vec3 n0, n1;
    double t0, t1;

    if(!bc){
        t1 = 1;
        n1 = (c->P() - c->next()->P()).normalize();
    }else{
        if (c->isC0()){
            n1 = Vec3(0,0,1)*(1-fabs(bc->T0)) + (bc->getP(1) - bc->getP(0))*(bc->T0);
            t1 = fabs(bc->T0);
        }else{
            n1 = Vec3(0,0,1)*(1-fabs(bc->T1)) + (bc->getP(2) - bc->getP(3))*bc->T1;
            t1 = fabs(bc->T1);
        }
    }

    Bezier* bc0 = (Bezier*)c->prev()->E()->curve;
    if(!bc0){
        t0 = 1;
        n0 = (c->P() - c->prev()->P()).normalize();
    }else{
        if (c->prev()->isC0()){
            n0 = Vec3(0,0,1)*(1-fabs(bc0->T1)) + (bc0->getP(2) - bc0->getP(3))*bc0->T1;
            t0 = fabs(bc0->T1);
        }else{
            n0 = Vec3(0,0,1)*(1-fabs(bc0->T0)) + (bc0->getP(1) - bc0->getP(0))*bc0->T0;
            t0 = fabs(bc0->T0);
        }
    }

    return ((n0*t0+n1*t1)/(t0+t1)).normalize();
}
Vec3 Patch4::quadNormal(const Vec3& p0, const Vec3& p1,const Vec3& p2,const Vec3& p3){
    return quadNormal2(p0, p1, p2, p3);
    /*
    switch(NMODE){
        case 1:
            return quadNormal1(p0, p1, p2, p3);
        case 0:
            return quadNormal2(p0, p1, p2, p3);
        case 1:
            return quadNormal3(p0, p1, p2, p3);
    }*/
