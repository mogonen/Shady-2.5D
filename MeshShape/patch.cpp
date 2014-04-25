 #include "Patch.h"

/*int     Patch::N;
int     Patch::Ni;
int     Patch::NN;
int     Patch::NN2;
double  Patch::T;*/
//bool    Patch::isH = true;
#define SAMPLES 11

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

void Patch::propateNormals(Point_p ps)
{

    Normal corner_ns[4];
    for(int i=0; i<_pFace->size(); i++)
        corner_ns[i] = computeN(C(i));

    Map map = _maps[NORMAL_CHANNEL];
    for(int i=0; i<_pFace->size(); i++)
    {
        int Ni = (i%2)? _sampleVi : _sampleUi;
        Vec3 n0 = corner_ns[i];
        Vec3 n1 = corner_ns[(i+1)%4];
        map[edgeInd(i, 0)] = n0;
        map[edgeInd(i, Ni)] = n1;

        Vec2 tan0 = (ps[edgeInd(i, 1)] - ps[edgeInd(i, 0)]).normalize();
        Vec2 tan1 = (ps[edgeInd(i, Ni)] - ps[edgeInd(i, Ni-1)]).normalize();

        Vec3 n0_d = Patch::decompose(n0, Vec3(tan0));
        Vec3 n1_d = Patch::decompose(n1, Vec3(tan1));

        for(int j=1; j<Ni; j++)
        {
            double t = j*(1.0/Ni);
            Vec3 n_d = n0_d*(1-t) + n1_d*(t);
            Vec3 tan = (ps[edgeInd(i, j+1)] - ps[edgeInd(i, j-1)]).normalize();
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

Vec3 Patch::decompose(const Vec3& v, const Vec3& nx)
{
    Vec3 nz(0, 0, 1);
    Vec3 ny = nz%nx;
    return Vec3(v*nx, v*ny, v*nz);
}

Vec3 Patch::compose(const Vec3& v, const Vec3& nx)
{
    Vec3 nz(0,0,1);
    Vec3 ny = nz%nx;
    return nx*v.x + ny*v.y + nz*v.z;
}

Patch4::Patch4(Face_p pF):Patch(pF)
{
    setSample(SAMPLES,SAMPLES);
    _ps = new Point[_sampleUV];

    for(int i=0; i<ACTIVE_CHANNELS; i++){
        Map map = new RGB[_sampleUV];
        _maps.push_back(map);
    }
}

void Patch4::onUpdate()
{
    _pFace->update();
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
    computeBezierPatch(_ps, K, _sampleU, _sampleV);
    //needs to be optimized
    for(int i =0; i< ACTIVE_CHANNELS; i++)
        interpolateMap(i);
    //}
}

RGB Patch4::interpolateCoonz(int i, int j, Map map, int U)
{
    int Ui =  U-1;

    double t =  H(i*1.0 / Ui); //isH? H(i*1.0 / Ui) : (i*1.0 / Ui);
    double s =  H(j*1.0 / Ui); //isH? H(j*1.0 / Ni) : (j*1.0 / Ni);

    RGB val = map[i]*(1.0-s) + map[i + Ui*U]*s;
    val = val + map[0 + j*U]*(1.0 - t) + map[Ui + j*U]*t;
    val = val - map[0]*(1-s)*(1.0-t) - map[Ui]*(1-s)*t - map[Ui + Ui*U]*s*t - map[Ui*U]*s*(1-t);
    return val;
}

void Patch4::computeBezierPatch(Point * ps, Point K[], int U, int V)
{
    double tu = 1.0 / (U-1);
    double tv = 1.0 / (V-1);

    for(int j = 0; j < V;j++)
    {
        for(int i = 0; i < U; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*tu)*cubicBernstein(bj, j*tv)*K[bi+bj*4];
            ps[i + j*U] = p;
        }
    }
}

void Patch4::interpolateMap(int channel)
{
    if (channel == NORMAL_CHANNEL)
        propateNormals(_ps);
    else
        propateMap(channel);

    Map map = _maps[channel];
    for(int j = 1; j<_sampleVi;j++)
        for(int i = 1; i<_sampleUi; i++)
                map[ind(i,j)] = interpolateCoonz(i, j, map, _sampleU);
}

int Patch4::edgeInd(int ei, int i){

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

int Patch4::edgeUInd(int ei, int i){

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

int Patch4::edgeI(int i, int j)
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

int Patch4::cornerI(int i, int j)
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

Point Patch4::KVal(int ei, int i){
    Corner* ci = C(ei);
    bool isforward = !ci->isC1();
    CurvedEdge* c = ci->E()->pData;
    bool rev = ((ei>1) && isforward) || ( (ei<2) && !isforward);
    return (rev)?c->CV(3-i):c->CV(i);
}

Normal Patch::computeN(Corner_p c)
{
    /*
    Point p = P(c);
    Point p1 = c->E()->curve->CV(c->isC0()?1:2);                    //next
    Point p0 = c->prev()->E()->curve->CV(c->prev()->isC0()?2:1);   //prev
    Point p2 = p0 + (p1-p);
    */
    Vec3 n = c->V()->pData->N();
    return n.z>0 ? c->V()->pData->N() : Vec3(n.x, n.y, N_MIN_Z);
}

void PatchN::onUpdate()
{
    //Point p0 = C(0)->E()->pData->getTangentSV(C(0));

    int nC = face()->size();
    Point* pKEdge = new Point[nC*6];
    Point* pKMid  = new Point[nC*2];

    Point pmid;
    for(int i = 0; i < nC; i++)
        pmid = pmid + P0(C(i));
    pmid = pmid * (1.0 / nC);

    for(int i = 0; i < nC; i++)
    {
        int i6 = i*6;
        Point CV[7];
        C(i)->E()->pData->computeSubdivisionCV(0.5, CV);
        pKEdge[i6+1] = CV[3];

        if (!C(i)->isC1()){
            pKEdge[i6]   = CV[2];
            pKEdge[i6+2] = CV[4];

            pKEdge[i6+4] = CV[1];
            pKEdge[i6+5] = CV[5];
        }else{
            pKEdge[i6]   = CV[4];
            pKEdge[i6+2] = CV[2];
            pKEdge[i6+4] = CV[5];
            pKEdge[i6+5] = CV[1];
        }

        double t = (CV[3] - pmid).norm()*0.25;
        Vec3 v = Vec3((pKEdge[i6+2] - pKEdge[i6]).normalize()) % Vec3(0,0,1);
        pKEdge[i6+3] = CV[3] + Vec2(v.x, v.y)*t;

        pKMid[i*2]   = pmid + (P0(C(i)) - pmid)*0.25;
        pKMid[i*2+1] = pmid + (CV[3] - pmid)*0.25;
    }

    Point * K = _K;
    for(int i = 0; i < face()->size(); i++)
    {
        Corner_p pC = C(i);
        int iE0  = ((i-1+nC)%nC)*6;
        int iE = i*6;

        K[0] = P0(pC);
        K[1] = pKEdge[iE+4];
        K[2] = pKEdge[iE+0];
        K[3] = pKEdge[iE+1];

        K[4] = pKEdge[iE0+5];
        K[7] = pKEdge[iE+3];

        K[8]  = pKEdge[iE0+2];
        K[10] = pKMid[i*2];
        K[11] = pKMid[i*2+1];

        K[12] = pKEdge[iE0+1];
        K[13] = pKEdge[iE0+3];
        K[14] = pKMid[((i-1+nC)%nC)*2+1];
        K[15] = pmid;

        K[5]   = K[1]  + K[4]  - K[0];
        K[6]   = K[2]  + K[7]  - K[3];
        K[9]   = K[13] + K[8]  - K[12];

        Patch4::computeBezierPatch(_ps + (_sampleUV)*i , K, _sampleU, _sampleV);
        //K[10]  = K[11] + K[14] - K[15];
    }

    for(int i = 0; i < nC; i++){

        for(int c = 0; c < ACTIVE_CHANNELS; c++){
        }
    }

}

PatchN::PatchN(Face_p pF):Patch(pF)
{
    setSample(SAMPLES/2+1, SAMPLES/2+1);
    int psize = pF->size()*_sampleUV;
    _ps = new Point[psize];

    for(int i = 0; i < ACTIVE_CHANNELS*pF->size(); i++)
    {
        Map map = new RGB[_sampleUV];
        _maps.push_back(map);
    }
}

