#include <qgl.h>
#include "pattern.h"
#include "../curve.h"

int     PatternPatch::NU = 6;
int     PatternPatch::NV = 6;
bool    UVPatternPatch::isTHICK = false;
//double  Patch::T;

#define FOLD_W 10

void PatternPatch::assignPattern(int uv, int off, int len, int * data)
{
    int end = (uv == 0)? _nU :_nV;
    for(int i = 0; i < end; i++ ){
        int p =  data[(off + i)%len];
        _pattern[i + uv*_nU] = p;
    }
 int i = 5;
}

GridPattern::GridPattern(Face_p pF):PatternPatch(pF){
    _ps = 0;
    init(PatternPatch::NU, PatternPatch::NV);
}

void GridPattern::init(int nu, int nv){

    _nU = nu;
    _nV = nv;

    if (_pattern)
        delete _pattern;

    setSample(nu? nu*FOLD_W+1 : FOLD_W*2, nv?nv*FOLD_W + 1 :FOLD_W*2);

    _pattern = new int[(_nU+_nV)];
    for(int i = 0; i < (_nU + _nV); i++)
        _pattern[i] = 0;

    if (_ps)
        delete _ps;
    _ps = new Point[_sampleUV];

}

int GridPattern::getPattern(int i, int j) const{

    double wu  = _sampleUi*1.0 /_nU;
    int ui  = i / wu;
    int uii = i -  (wu * ui);

    double margin_u = 3;//wu*0.1;

    int result = -1;
    if ( uii >= margin_u && uii < (wu-margin_u))
    {
        if (_pattern[ui]>0)
            return _pattern[ui];
        else
            result = _pattern[ui];
    }

    double  wv  =  _sampleVi*1.0 /_nV;
    int vj  = j / wv;
    int vjj = j -  (wv * vj);

    int margin_v = 3;//wv*0.25;

    if ( (vjj >= margin_v) && (vjj < (wv-margin_v)) )
    {
        return _pattern[_nU + vj];
    }

    return result;
}


void GridPattern::onUpdate()
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

    double tu = 1.0 / (_sampleUi);
    double tv = 1.0 / (_sampleVi);

    for(int j = 0; j < _sampleV;j++)
    {
        for(int i = 0; i < _sampleU; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*tu)*cubicBernstein(bj, j*tv)*K[bi+bj*4];
            _ps[i+j*_sampleU] = p;
        }
    }
}

void GridPattern::render(int mode){

    if (!Session::isRender(DRAG_ON))
      Selectable::render(mode);

      for(int j=0; j < _sampleVi; j++){
        for(int i = 0; i< _sampleUi; i++){

            Point p[4];
            p[0] = P(i, j);
            p[1] = P(i+1, j);
            p[2] = P(i+1, j+1);
            p[3] = P(i, j+1);

            int pat = getPattern(i,j);

            glColor3f(0.95, 0.95, 0.95);
            if (pat == 0)
                glColor3f(0.90, 0.90, 0.90);
            else if (pat == 1)
                glColor3f(0.9, 0.1, 0.1);
            else if (pat == 2)
                glColor3f(0.1, 0.9, 0.1);

            if (Session::isRender(WIREFRAME_ON))
                glBegin(GL_LINE_LOOP);
            else
                glBegin(GL_POLYGON);
            for(int k = 0; k < 4; k++){
                glVertex2f(p[k].x, p[k].y);
            }
            glEnd();
        }
    }
}


UVPatternPatch::UVPatternPatch(Face_p pF):PatternPatch(pF){
    setSample(20,20);
    _ps = 0;
    _pattern = 0;
    init(PatternPatch::NU, PatternPatch::NV);
}

void UVPatternPatch::init(int nu, int nv)
{
    _nU = nu;
    _nV = nv;

    if (_pattern)
        delete _pattern;

    _pattern = new int[_nU+_nV];
    for(int i = 0; i < _nU + _nV; i++)
        _pattern[i] = 0;

    if (_ps)
        delete _ps;

    _W = (isTHICK?2:1);
    _ps = new Point[(_nU*_sampleU + _nV*_sampleV)*_W];//
}

int  UVPatternPatch::getPattern(int i, int uv) const{
    return _pattern[i + uv*_nU];
}

/*
void UVPatternPatch::onUpdate(){

    _pFace->update();

    //init bezier surface points
    for(int i=0; i<4; i++)
    {
        K[i]= KVal(0,i);
        K[i+12]= KVal(2,i);
    }

    K[7] = KVal(1,1);
    K[11] = KVal(1,2);

    K[4] = KVal(3,1);
    K[8] = KVal(3,2);

    K[5]  = K[1]  + K[4]  - K[0];
    K[6]  = K[2]  + K[7]  - K[3];
    K[9]  = K[13] + K[8]  - K[12];
    K[10] = K[11] + K[14] - K[15];

    Point* samples = new Point[N];

    double Tu = 1.0 / _nU, Tv = 1.0 /_nV;
    for(int u = 0; u < _nU; u++)
    {
        for(int i=0; i<N; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*T)*cubicBernstein(bj, (u+1)*Tu)*K[bi+bj*4];

            samples[i] = p;
        }

        for(int i=0; i<N; i++)
        {
            int i0 = (i-1)>=0?(i-1):0;
            int i1 = (i+1)<N?(i+1):Ni;

            Vec2 tan = samples[i1] - samples[i0];
            Vec3 n3d = Vec3(tan)%Vec3(0,0,1);
            Vec2 n2d(n3d.x, n3d.y);
            n2d = n2d.normalize()*0.01;
            _ps[ind(0,u,i,0)] = samples[i] - n2d;
            _ps[ind(0,u,i,1)] = samples[i] + n2d;

        }
    }

    for(int v = 0; v < _nV; v++)
    {
        for(int i=0; i<N; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, (v+1)*Tv)*cubicBernstein(bj, i*T)*K[bi+bj*4];

            samples[i] = p;
        }

        for(int i=0; i<N; i++)
        {
            int i0 = (i-1)>=0?(i-1):0;
            int i1 = (i+1)<N?(i+1):Ni;

            Vec2 tan = samples[i1] - samples[i0];
            Vec3 n3d = Vec3(tan)%Vec3(0,0,1);
            Vec2 n2d(n3d.x, n3d.y);
            n2d = n2d.normalize()*0.01;
            _ps[ind(1,v,i,0)] = samples[i] - n2d;
            _ps[ind(1,v,i,1)] = samples[i] + n2d;

        }

    }
    delete samples;
}
*/

void UVPatternPatch::onUpdate(){

    /*if (_nU != NU || _nV != NV)
        init(NU, NV);*/

    face()->update();

    Point K[16];
    //init bezier surface points
    for(int i=0; i<4; i++)
    {
        K[i]= KVal(0,i);
        K[i+12]= KVal(2,i);
    }

    K[7] = KVal(1,1);
    K[11] = KVal(1,2);

    K[4] = KVal(3,1);
    K[8] = KVal(3,2);

    K[5]  = K[1]  + K[4]  - K[0];
    K[6]  = K[2]  + K[7]  - K[3];
    K[9]  = K[13] + K[8]  - K[12];
    K[10] = K[11] + K[14] - K[15];

    //bezier surface interpolation
    /*
     *for(int j=0; j<N;j++)
        for(int i=0; i<N; i++){
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*T)*cubicBernstein(bj, j*T)*K[bi+bj*4];
            _ps[ind(i,j)] = p;
        }
    */

    double Tu = 1.0 / _nU, Tv = 1.0 /_nV;
    double Wu = Tu*0.1*isTHICK;
    double Wv = Tv*0.1*isTHICK;

    for(int u = 0; u < _nU; u++)
    {
        for(int i=0; i < _sampleU; i++)
        {
            Point p0, p1;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++){
                    p0 = p0 + cubicBernstein(bi, i*_Tu)*cubicBernstein(bj, (u+0.5)*Tu - Wu)*K[bi+bj*4];
                    if (isTHICK)
                        p1 = p1 + cubicBernstein(bi, i*_Tu)*cubicBernstein(bj, (u+0.5)*Tu + Wu)*K[bi+bj*4];
                }

            _ps[ind(0,u,i,0)] = p0;
            if (isTHICK)
                _ps[ind(0,u,i,1)] = p1;
        }

    }

    for(int v = 0; v < _nV; v++)
    {
        for(int i=0; i<_sampleV; i++)
        {
            Point p0, p1;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++){
                    p0 = p0 + cubicBernstein(bi, (v+0.5)*Tv - Wv)*cubicBernstein(bj, i*_Tv)*K[bi+bj*4];
                    if (isTHICK)
                        p1 = p1 + cubicBernstein(bi, (v+0.5)*Tv + Wv)*cubicBernstein(bj, i*_Tv)*K[bi+bj*4];
                }

            _ps[ind(1,v,i,0)] = p0;
            if (isTHICK)
                _ps[ind(1,v,i,1)] = p1;
        }

    }
}

void UVPatternPatch::render(int mode)
{
    if (!Session::isRender(DRAG_ON))
        Selectable::render(mode);

    if (isTHICK){

        for(int u=0; u < _nU; u++)
        {
            if (_pattern[u] == 1)
                glColor3f(0, 1.0 ,0);
            else if (_pattern[u] == 2)
                glColor3f(1.0, 0 ,0);
            else
                glColor3f(0.8, 0.8, 0.8);

            for(int i = 0; i< _sampleUi; i++)
            {
                Point p[4] = {P(0, u, i), P(0, u, i,1), P(0, u, i+1, 1), P(0, u, i+1, 0)};
                if (Session::isRender(WIREFRAME_ON))
                    glBegin(GL_LINE_LOOP);
                else
                    glBegin(GL_POLYGON);
                for(int k = 0; k<4; k++ ){
                    glVertex3f(p[k].x, p[k].y, 0);
                }
                glEnd();
            }
        }

        //glColor3f(1.0, 0, 0);
        for(int v=0; v < _nV; v++)
        {
            if (_pattern[_nU + v] == 1)
                glColor3f(0, 1.0 ,0);
            else if (_pattern[_nU + v] == 2)
                glColor3f(1.0, 0 ,0);
            else
                glColor3f(0.8, 0.8, 0.8);

            for(int i = 0; i< _sampleVi; i++)
            {
                Point p[4] = {P(1, v, i), P(1, v, i, 1), P(1, v, i+1, 1), P(1, v, i+1, 0)};
                if (Session::isRender(WIREFRAME_ON))
                    glBegin(GL_LINE_LOOP);
                else
                    glBegin(GL_POLYGON);
                for(int k = 0; k<4; k++ ){
                    glVertex3f(p[k].x, p[k].y, 0);
                }
                glEnd();
            }
        }
    }else{

        for(int u=0; u < _nU; u++)
        {
            if (_pattern[u] == 1)
                glColor3f(0, 1.0 ,0);
            else if (_pattern[u] == 2)
                glColor3f(1.0, 0 ,0);
            else
                glColor3f(0.8, 0.8, 0.8);

            glBegin(GL_LINE_STRIP);
            for(int i = 0; i< _sampleU; i++)
            {
                Point p = P(0, u, i);
                glVertex3f(p.x, p.y, 0);
            }
            glEnd();

        }

        //glColor3f(1.0, 0, 0);
        for(int v=0; v < _nV; v++)
        {
            if (_pattern[_nU + v] == 1)
                glColor3f(0, 1.0 ,0);
            else if (_pattern[_nU + v] == 2)
                glColor3f(1.0, 0 ,0);
            else
                glColor3f(0.8, 0.8, 0.8);

            glBegin(GL_LINE_STRIP);
            for(int i = 0; i < _sampleV; i++)
            {
                Point p = P(1, v, i);
                glVertex3f(p.x, p.y, 0);
            }
            glEnd();
        }
    }
}
/*

void UVPatternPatch::render(int mode)
{
    if (!Session::isRender(DRAG_ON))
        Patch::render(mode);

    for(int u=0; u < _nU; u++)
    {
        if (_pattern[u] == 1)
            glColor3f(0, 1.0 ,0);
        else if (_pattern[u] == 2)
            glColor3f(1.0, 0 ,0);
        else
            glColor3f(0.8, 0.8, 0.8);

        glBegin(GL_LINE_STRIP);
        for(int i = 0; i< N; i++)
        {
            Point p = P(0, u, i);
            glVertex3f(p.x, p.y, 0);
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        for(int i = 0; i< N; i++)
        {
            Point p = P(0, u, i, 1);
            glVertex3f(p.x, p.y, 0);
        }
        glEnd();
    }

    //glColor3f(1.0, 0, 0);

    for(int v=0; v < _nV; v++)
    {
        if (_pattern[_nU + v] == 1)
            glColor3f(0, 1.0 ,0);
        else if (_pattern[_nU + v] == 2)
            glColor3f(1.0, 0 ,0);
        else
            glColor3f(0.8, 0.8, 0.8);

        glBegin(GL_LINE_STRIP);
        for(int i = 0; i < N; i++)
        {
            Point p = P(1, v, i);
            glVertex3f(p.x, p.y, 0);
        }
        glEnd();

        glBegin(GL_LINE_STRIP);
        for(int i = 0; i < N; i++)
        {
            Point p = P(1, v, i, 1);
            glVertex3f(p.x, p.y, 0);
        }
        glEnd();
    }

}
*/
