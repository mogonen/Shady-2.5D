#include "meshshape.h"

double   MeshShape::GRID_LEN = 0.2;
int     MeshShape::GRID_N  = 2;
int     MeshShape::GRID_M  = 2;
int     MeshShape::NGON_N  = 3;
int     MeshShape::NGON_SEG_V   = 1;
double  MeshShape::NGON_RAD     = 0.2;
int     MeshShape::TORUS_N      = 8;
double  MeshShape::TORUS_RAD    = 0.2;

MeshShape* MeshShape::insertGrid(const Point& p, double len, int n, int m, MeshShape *pMS){

    if (!pMS)
        pMS = new MeshShape();

    Point p0 = p - Point(len*n/2, -len*m/2);

    m = m?m:n;
    Vertex_p* vs = new Vertex_p[(n+1)*(m+1)];
    for(int j=0; j<m+1; j++)
        for(int i=0; i<n+1; i++)
            vs[i+j*(n+1)] = pMS->addMeshVertex(p0+Point(len*i,0)+ Point(0,-len*j));

    for(int j=0; j<m; j++)
        for(int i=0; i<n; i++)
            pMS->_control->addQuad(vs[i+j*(n+1)], vs[i+1+j*(n+1)], vs[i+1+(j+1)*(n+1)], vs[i+(j+1)*(n+1)] );

    delete vs;
    pMS->_control->buildEdges();
    pMS->makeSmoothTangents();
    pMS->Renderable::update();
    return pMS;
}

MeshShape* MeshShape::insertNGon(const Point& p, int n, int segv, double rad, MeshShape *pMS){

    if (!pMS)
        pMS = new MeshShape();

    CLAMP(segv, 1, 4);
    CLAMP(n, 2, 8);

    int nn = 2*n;
    Vertex_p vmid = pMS->addMeshVertex();
    Vertex_p* vs = new Vertex_p[nn*segv];

    for(int j=0; j<segv; j++){
        for(int i=0; i<nn; i++){
            double ang = -i*PI/n;
            double angv = j*PI/(2*segv);
            vs[i + j*nn] = pMS->addMeshVertex(p + Point(rad*cos(ang), rad*sin(ang))*cos(angv));
        }
    }

    if (segv > 1){
        for(int j=0; j<segv-1; j++){
            for(int i=0; i<nn; i++){
                pMS->_control->addQuad(vs[i + j*nn], vs[(i+1)%nn + j*nn], vs[(i+1)%nn + (j+1)*nn], vs[i + (j+1)*nn] );
            }

        }
    }

    int mid_off = (segv-1)*nn;
    for(int i=0; i<n; i++)
        pMS->_control->addQuad(vmid, vs[i*2+mid_off], vs[i*2+1+mid_off], vs[(i*2+2)%nn+mid_off]);

    delete vs;

    pMS->_control->buildEdges();
    pMS->makeSmoothTangents();
    pMS->Renderable::update();
    return pMS;

}

MeshShape* MeshShape::insertTorus(const Point& p, int n, double rad, MeshShape* pMS){

    if (!pMS)
        pMS = new MeshShape();

    int segU = 12;
    int segV = 3;

    double step_u = 2*PI / segU;
    Vertex_p* vs = new Vertex_p[segU*segV];


    FOR_ALL_J(segV){
        FOR_ALL_I(segU){
            double ang_u = -step_u * i;
            Point p(cos(ang_u)*rad, sin(ang_u)*rad);
            vs[i+j*segU] = pMS->addMeshVertex(p*(1-0.6/segV*j));
        }
    }

    FOR_ALL_J(segV-1){
        FOR_ALL_I(segU){
            pMS->_control->addQuad(vs[i+j*segU] , vs[(i+1)%segU + j*segU], vs[(i+1)%segU + (j+1)*segU], vs[i + (j+1)*segU]);
        }
    }

    delete vs;
    pMS->_control->buildEdges();
    pMS->makeSmoothTangents();
    pMS->Renderable::update();
    return pMS;
}
