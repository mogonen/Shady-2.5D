#include "meshshape.h"

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

MeshShape* MeshShape::insertNGon(const Point& p, int n, double rad, MeshShape *pMS){

    if (!pMS)
        pMS = new MeshShape();

    int nn = 2*n;
    Vertex_p vmid = pMS->addMeshVertex();
    Vertex_p* vs = new Vertex_p[nn];

    for(int i=0; i<nn; i++){
        double ang = -i*PI/n;
        vs[i] = pMS->addMeshVertex(p + Point(1,0)*rad*cos(ang) + Point(0,1)*rad*sin(ang) );
    }

    for(int i=0; i<n; i++)
        pMS->_control->addQuad(vmid, vs[i*2], vs[i*2+1], vs[(i*2+2)%nn]);

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
