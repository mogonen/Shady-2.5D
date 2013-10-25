#include "meshshape.h"

MeshShape* MeshShape::insertGrid(const Point& p, double len, int n, int m, MeshShape *pMS){

    if (!pMS)
        pMS = new MeshShape();

    m = m?m:n;
    Vertex_p* vs = new Vertex_p[(n+1)*(m+1)];
    for(int j=0; j<m+1; j++)
        for(int i=0; i<n+1; i++)
            vs[i+j*(n+1)] = pMS->addMeshVertex(p+Point(len*i,0)+ Point(0,-len*j));

    for(int j=0; j<m; j++)
        for(int i=0; i<n; i++)
            pMS->_control->addQuad(vs[i+j*(n+1)], vs[i+1+j*(n+1)], vs[i+1+(j+1)*(n+1)], vs[i+(j+1)*(n+1)] );

    delete vs;
    pMS->_control->buildEdges();
    pMS->centerPivot();
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
