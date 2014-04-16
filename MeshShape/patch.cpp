 #include "Patch.h"

/*int     Patch::N;
int     Patch::Ni;
int     Patch::NN;
int     Patch::NN2;
double  Patch::T;*/
bool    Patch::isH = true;
#define SAMPLES 10

static const int FTABLE[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600};
inline double cubicBernstein(int i, double t){
    return 6.0 / (FTABLE[3-i] * FTABLE[i]) * pow(1-t, 3-i) * pow(t,i);
}

Patch::Patch(Face_p pF):Selectable(false){
    _ps = 0;
    _pFace = pF;
    _pFace->pData = this;
    setRef(pF);
}

void Patch::propateNormals(){

    Normal corner_ns[4];
    for(int i=0; i<_pFace->size(); i++)
        corner_ns[i] = computeN(C(i));

    Map map = _maps[NORMAL_CHANNEL];

    for(int i=0; i<_pFace->size(); i++){

        int Ni = (i%2)? _sampleVi : _sampleUi;

        Vec3 n0 = corner_ns[i];
        Vec3 n1 = corner_ns[(i+1)%4];
        map[edgeInd(i, 0)] = n0;
        map[edgeInd(i, Ni)] = n1;

        Vec2 tan0 = (_ps[edgeInd(i, 1)] - _ps[edgeInd(i, 0)]).normalize();
        Vec2 tan1 = (_ps[edgeInd(i, Ni)] - _ps[edgeInd(i, Ni-1)]).normalize();

        Vec3 n0_d = Patch::decompose(n0, Vec3(tan0));
        Vec3 n1_d = Patch::decompose(n1, Vec3(tan1));

        for(int j=1; j<Ni; j++)
        {
            double t = j*(1.0/Ni);
            Vec3 n_d = n0_d*(1-t) + n1_d*(t);
            Vec3 tan = (_ps[edgeInd(i, j+1)] - _ps[edgeInd(i, j-1)]).normalize();
            map[edgeInd(i, j)] = compose(n_d, tan);
        }
    }
}

void Patch::propateMap(int channel){

    Map map = _maps[channel];

    RGB corner_col[4];
    for(int i=0; i<_pFace->size(); i++)
    {
        corner_col[i] = C(i)->V()->pData->value[channel];
    }

    for(int i=0; i<_pFace->size(); i++)
    {
        int Ni = (i%2)? _sampleVi : _sampleUi;
        RGB c0 = corner_col[i];
        RGB c1 = corner_col[(i+1)%4];
        map[edgeInd(i, 0)]  = c0;
        map[edgeInd(i, Ni)] = c1;

        for(int j=1; j<Ni; j++)
        {
            double t = j*(1.0/Ni);
            map[edgeInd(i, j)] = c0*(1-t) + c1*t;
        }
    }
}

Patch4::Patch4(Face_p pF):Patch(pF){
    setSample(SAMPLES,SAMPLES);
    _ps = new Point[_sampleUV];

    for(int i=0; i<ACTIVE_CHANNELS; i++){
        Map map = new RGB[_sampleUV];
        _maps.push_back(map);
    }
}


RGB Patch4::interpolate(int i, int j, Map map)
{
    int Ni = _sampleUi;
    int N  = _sampleU;

    double t =  H(i*1.0 / Ni); //isH? H(i*1.0 / Ni) : (i*1.0 / Ni);
    double s =  H(j*1.0 / Ni); //isH? H(j*1.0 / Ni) : (j*1.0 / Ni);

    RGB val = map[i]*(1.0-s) + map[i + Ni*N]*s;
    val = val + map[0 + j*_sampleU]*(1.0 - t) + map[Ni + j*_sampleU]*t;
    val = val - map[0]*(1-s)*(1.0-t) - map[Ni]*(1-s)*t - map[Ni + Ni*N]*s*t - map[Ni*N]*s*(1-t);
    return val;
}

void Patch4::onUpdate(){

    _pFace->update();

    //init bezier surface points
    for(int i=0; i<4; i++){
        _K[i]    = K(0,i);
        _K[i+12] = K(2,i);
    }

    _K[7]   = K(1,1);
    _K[11]  = K(1,2);

    _K[4]   = K(3,1);
    _K[8]   = K(3,2);

    _K[5]   = _K[1]  + _K[4]  - _K[0];
    _K[6]   = _K[2]  + _K[7]  - _K[3];
    _K[9]   = _K[13] + _K[8]  - _K[12];
    _K[10]  = _K[11] + _K[14] - _K[15];

    //bezier surface interpolation
    for(int j=0; j<_sampleV;j++)
        for(int i=0; i<_sampleU; i++){
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*_Tu)*cubicBernstein(bj, j*_Tv)*_K[bi+bj*4];
            _ps[ind(i,j)] = p;
        }

    //needs to be optimized
    for(int i =0; i< ACTIVE_CHANNELS; i++)
        interpolateMap(i);
    //}
}
/*
void Patch4::interpolateNormals(){
    for(int j = 1; j<_sampleVi;j++)
        for(int i = 1; i<_sampleUi; i++)
                _ns[ind(i,j)] = interpolateN(i, j);
}
*/
void Patch4::interpolateMap(int channel)
{
    if (channel == NORMAL_CHANNEL)
        propateNormals();
    else
        propateMap(channel);

    Map map = _maps[channel];
    for(int j = 1; j<_sampleVi;j++)
        for(int i = 1; i<_sampleUi; i++)
                map[ind(i,j)] = interpolate(i, j, map);
}

int Patch::edgeInd(int ei, int i){

    int N = (ei%2) ? _sampleV : _sampleU;
    int Ni  = N-1;

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

    int N = (ei%2) ? _sampleV : _sampleU;
    int Ni  = N-1;

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

int Patch::edgeI(int i, int j){

    //make i U dominant

    if (j == 0 )
        return 0;

    if (j == _sampleVi)
        return 2;

    if ( i == 0)
        return 3;

    if (i == _sampleUi)
        return 1;

    /*if (j == 0 && i < _sampleUi)
        return 0;

    if ( i == _sampleUi && j < _sampleVi)
        return 1;

    if ( i > 0 && j == _sampleVi)
        return 2;

    if ( i == 0 && j < 0)
        return 3;
    */

    return -1;
}

int Patch::cornerI(int i, int j){
    if (j == 0 && i == 0)
        return 0;

    if ( i == _sampleUi && j == 0)
        return 1;

    if ( i == _sampleUi && j == _sampleVi)
        return 2;

    if ( i == 0 && j == _sampleVi)
        return 3;
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
    bool isforward = !ci->isC1();
    CurvedEdge* c = ci->E()->pData;
    bool rev = ((ei>1) && isforward) || ( (ei<2) && !isforward);
    return (rev)?c->CV(3-i):c->CV(i);
}

Normal Patch::computeN(Corner_p c)
{
    /*Point p = P(c);
    Point p1 = c->E()->curve->CV(c->isC0()?1:2);				//next
    Point p0 = c->prev()->E()->curve->CV(c->prev()->isC0()?2:1);   //prev
    Point p2 = p0 + (p1-p);*/
    Vec3 n = c->V()->pData->N();
    return n.z>0 ? c->V()->pData->N() : Vec3(n.x, n.y, N_MIN_Z);
}

/*
Normal Patch::computeN(Corner_p c0, double t)
{
    bool isforward = (c0->V()->pData->pP() == c0->E()->pData->pCV(0));
    t = isforward? t :(1-t);

    Normal n0 = computeN(c0);
    Normal n1 = computeN(c0->next());

    Vec2 tan0 = c0->E()->pData->evalT(0);//(_ps[edgeInd(i, 1)] - _ps[edgeInd(i, 0)]).normalize();
    Vec2 tan1 = c0->E()->pData->evalT(1);//(_ps[edgeInd(i, Ni)] - _ps[edgeInd(i, Ni-1)]).normalize();

    Vec3 n0_d = Patch::decompose(n0, Vec3(tan0));
    Vec3 n1_d = Patch::decompose(n1, Vec3(tan1));

    Vec3 n_d = n0_d*(1-t) + n1_d*(t);
    Vec3 tan = c0->E()->pData->evalT(t);
    return compose(n_d, tan);
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
