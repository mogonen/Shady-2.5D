 #include "Patch.h"

#define SAMPLES 11
static const int FTABLE[] = {1, 1, 2, 6, 24, 120, 720, 5040, 40320, 362880, 3628800, 39916800, 479001600};
inline double cubicBernstein(int i, double t)
{
    return 6.0 / (FTABLE[3-i] * FTABLE[i]) * pow(1-t, 3-i) * pow(t,i);
}

Patch::Patch(Face_p pF):Selectable(false)
{
    _pFace = pF;
    if (pF){
        _pFace->pData = this;
        setRef(pF);
    }
#ifndef SHOW_DLFL
    //setSample(0, 0);
    //_map = 0;
    setSample(9, 9);
    _map = new ShapeVec[_sampleUi*_sampleVi];
#endif
}

void Patch::setSample(int u, int v){
    u = (u<2)?2:u;
    v = (v<2)?2:v;
   _sampleU = u;
   _sampleV = v;
   _sampleUi = u-1;
   _sampleVi = v-1;
   _sampleUV = u*v;
   _Tu = 1.0 / _sampleUi;
   _Tv = 1.0 / _sampleVi;
}

Patch::~Patch(){
    if (_map)
        delete _map;
}

Corner*  Patch::C(int i) const {
    if (_pFace->size()==3 && i>0) i--;
    return _pFace->C(i);
}

ShapeVec Patch::CVec(int i){
    return C(i)->E()->pData->get(C(i)->isC1()?C(i)->E()->pData->size()-1:0);
}

ShapeVec Patch::get(int i, int j){

    if (j==0)
    {
        if (_pFace->size()==3)
            return CVec(0);
        return C(0)->E()->pData->get(C(0)->isC1() ? (_sampleUi-i):i);
    }
    else if (j == _sampleVi)
    {
        return C(2)->E()->pData->get(C(2)->isC1() ? i : (_sampleUi-i));
    }
    else if (i == 0)
    {
        return C(3)->E()->pData->get(C(3)->isC1() ? j : (_sampleVi-j));
    }
    else if (i == _sampleUi)
    {
        return C(1)->E()->pData->get(C(1)->isC1() ? (_sampleVi-j):j);
    }
    else
        return _map[ind(i,j)];
}

Point Patch::P(int i, int j)
{
    return get(i,j)._P;
}

void Patch::updateBezierPatch(){
    Point K[16];

    //init bezier surface points
    for(int i = 0; i<4; i++)
    {
        K[i]    = KVal(0,i);
        K[i+12] = KVal(2,i);
    }

    K[7]   = KVal(1,1);
    K[11]  = KVal(1,2);

    K[4]   = KVal(3,1);
    K[8]   = KVal(3,2);

    K[5]   = K[1]  + K[4]  - K[0];
    K[6]   = K[2]  + K[7]  - K[3];
    K[9]   = K[13] + K[8]  - K[12];
    K[10]  = K[11] + K[14] - K[15];

    //bezier surface interpolation
    computeBezierPatch(K);
}

void Patch::adjustMapSize(){
    //check samples
    if (_sampleU != C(2)->E()->pData->size() || _sampleV != C(1)->E()->pData->size())
    {
        setSample(C(2)->E()->pData->size(), C(1)->E()->pData->size());
        if (_map)
            delete _map; //array del?
        _map = new ShapeVec[_sampleUi*_sampleVi];
    }
}

void Patch::onUpdate()
{
    _pFace->update();
    //adjustMapSize();
#ifndef SHOW_DLFL
    updateBezierPatch();
    interpolateMap();
#endif
}

void Patch::interpolateMap()
{
    int Ui =  _sampleU-1;
    for(int j = 1; j<_sampleVi;j++)
        for(int i = 1; i<_sampleUi; i++)
        {
            double t =  H(i*1.0 / Ui);
            double s =  H(j*1.0 / Ui);

            ShapeVec v_u0 = (_pFace->size()==3)? CVec(0):C(0)->E()->pData->get(C(0)->isC1()?(_sampleUi-i):i);
            ShapeVec v_u1 = C(2)->E()->pData->get(C(2)->isC1()? i : (_sampleUi-i));
            ShapeVec v_v0 = C(3)->E()->pData->get(C(3)->isC1()? j :(_sampleVi-j));
            ShapeVec v_v1 = C(1)->E()->pData->get(C(1)->isC1()? (_sampleVi-j):j);

            ShapeVec vec = v_u0*(1.0-s) + v_u1*s + v_v0*(1.0-t) + v_v1*t;
            vec = vec - CVec(0)*(1-s)*(1.0-t) - CVec(1)*(1-s)*t - CVec(2)*s*t - CVec(3)*s*(1 - t);
            _map[ind(i,j)] = vec;
        }
}

void Patch::computeBezierPatch(Point K[])
{
    int U = _sampleU;
    int V = _sampleV;

    double tu = 1.0 / (U-1);
    double tv = 1.0 / (V-1);

    for(int j = 1; j < V-1;j++)
    {
        for(int i = 1; i < U-1; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*tu)*cubicBernstein(bj, j*tv)*K[bi+bj*4];
                _map[ind(i,j)]._P = p;
        }
    }
}

Point Patch::KVal(int ei, int i)
{

    Corner* ci = C(ei);
    if (face()->size() == 3){
            if (ei%4 == 0 )
                return P0(C(0));
    }

    bool isforward = !ci->isC1();
    CurvedEdge* c = ci->E()->pData;
    bool rev = ((ei>1) && isforward) || ( (ei<2) && !isforward);
    return (rev)?c->CV(3-i):c->CV(i);
}

Point Patch::edgeP(int ei, int i) const{
    return _map[edgeInd(ei, i)]._P;
}

//Do we need this stuff anymore?

int Patch::edgeInd(int ei, int i) const{

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

int Patch::edgeUInd(int ei, int i) const{

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

int Patch::edgeI(int i, int j)
{

    //make i U dominant

    if (j == 0 )
        return 0;

    if (j == _sampleVi)
        return 2;

    if ( i == 0)
        return 3;

    if (i == _sampleUi)
        return 1;

    return -1;
}

int Patch::cornerI(int i, int j)
{
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
