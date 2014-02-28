#include <qgl.h>
#include "Patch.h"
#include "../curve.h"


int     PatternPatch::NU = 4;
int     PatternPatch::NV = 4;
//double  Patch::T;

PatternPatch::PatternPatch(Face_p pF):Patch(pF){
    _pattern = 0;
    init(NU, NV);
}

void PatternPatch::init(int nu, int nv)
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

    setN(10);
    _ps = new Point[_nU*_nV*N*2*2];
}

/*
void PatternPatch::onUpdate(){

    _pFace->update();

    //init bezier surface points
    for(int i=0; i<4; i++)
    {
        _K[i]= K(0,i);
        _K[i+12]= K(2,i);
    }

    _K[7] = K(1,1);
    _K[11] = K(1,2);

    _K[4] = K(3,1);
    _K[8] = K(3,2);

    _K[5]  = _K[1]  + _K[4]  - _K[0];
    _K[6]  = _K[2]  + _K[7]  - _K[3];
    _K[9]  = _K[13] + _K[8]  - _K[12];
    _K[10] = _K[11] + _K[14] - _K[15];

    Point* samples = new Point[N];

    double Tu = 1.0 / _nU, Tv = 1.0 /_nV;
    for(int u = 0; u < _nU; u++)
    {
        for(int i=0; i<N; i++)
        {
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*T)*cubicBernstein(bj, (u+1)*Tu)*_K[bi+bj*4];

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
                    p = p + cubicBernstein(bi, (v+1)*Tv)*cubicBernstein(bj, i*T)*_K[bi+bj*4];

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
void PatternPatch::onUpdate(){

    /*if (_nU != NU || _nV != NV)
        init(NU, NV);
*/
    _pFace->update();

    //init bezier surface points
    for(int i=0; i<4; i++)
    {
        _K[i]= K(0,i);
        _K[i+12]= K(2,i);
    }

    _K[7] = K(1,1);
    _K[11] = K(1,2);

    _K[4] = K(3,1);
    _K[8] = K(3,2);

    _K[5]  = _K[1]  + _K[4]  - _K[0];
    _K[6]  = _K[2]  + _K[7]  - _K[3];
    _K[9]  = _K[13] + _K[8]  - _K[12];
    _K[10] = _K[11] + _K[14] - _K[15];

    //bezier surface interpolation
    /*
     *for(int j=0; j<N;j++)
        for(int i=0; i<N; i++){
            Point p;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++)
                    p = p + cubicBernstein(bi, i*T)*cubicBernstein(bj, j*T)*_K[bi+bj*4];
            _ps[ind(i,j)] = p;
        }*/

    double Tu = 1.0 / _nU, Tv = 1.0 /_nV;
    double Wu = Tu*0.1;
    double Wv = Tv*0.1;

    for(int u = 0; u < _nU; u++)
    {
        for(int i=0; i<N; i++)
        {
            Point p0, p1;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++){
                    p0 = p0 + cubicBernstein(bi, i*T)*cubicBernstein(bj, (u+0.5)*Tu - Wu)*_K[bi+bj*4];
                    p1 = p1 + cubicBernstein(bi, i*T)*cubicBernstein(bj, (u+0.5)*Tu + Wu)*_K[bi+bj*4];
                }

            _ps[ind(0,u,i,0)] = p0;
            _ps[ind(0,u,i,1)] = p1;
        }

    }

    for(int v = 0; v < _nV; v++)
    {
        for(int i=0; i<N; i++)
        {
            Point p0, p1;
            for(int bj = 0; bj<4; bj++)
                for(int bi = 0; bi<4; bi++){
                    p0 = p0 + cubicBernstein(bi, (v+0.5)*Tv - Wv)*cubicBernstein(bj, i*T)*_K[bi+bj*4];
                    p1 = p1 + cubicBernstein(bi, (v+0.5)*Tv + Wv)*cubicBernstein(bj, i*T)*_K[bi+bj*4];
                }

            _ps[ind(1,v,i,0)] = p0;
            _ps[ind(1,v,i,1)] = p1;
        }

    }
}

void PatternPatch::assignPattern(int uv, int off, int len, int * data)
{
    int end = (uv == 0)? _nU :_nV;
    for(int i = 0; i < end; i++ ){
        int p =  data[(off + i)%len];
        _pattern[i + uv*_nU] = p;
    }
}

void PatternPatch::render(int mode)
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

        for(int i = 0; i< Ni; i++)
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

        for(int i = 0; i< Ni; i++)
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

}
/*

void PatternPatch::render(int mode)
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
