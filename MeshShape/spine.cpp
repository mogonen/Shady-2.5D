#include "spineshape.h"

typedef dlfl::Vertex*** OutlineMap;

double SpineShape::RAD = 0.1;

void SpineShape::onClick(const Point & p, Click_e eClick){

    if (eClick == DOWN || eClick == R_DOWN)
        return;

    //This could be improved
    SVertex_p pV = 0;

    for(SVertexList::iterator it = _verts.begin(); it != _verts.end(); it++)
        if ( (p-(*it)->P()).norm() < 0.015 ){
            pV = *it;
            break;
        }

    if (pV && eClick == UP)
        _lastV = 0;

    if (!pV){
        ShapeVertex_p pSV = addVertex(p);
        pV = new SVertex();
        pV->pSV = pSV;
        _verts.push_back(pV);
       // getController()->addControl(pP);
    }

    if (_lastV){//double link
        _lastV->links.push_back(pV);
        pV->links.push_back(_lastV);
    }

    _lastV = pV;
}

dlfl::Vertex_p* SpineShape::getOutlineVerts(const Point& o, const Point& tan, MeshShape *pMS, double rad){
    Point n = ( Vec3(0,0,1) % Vec3(tan.x, tan.y, 0) ).normalize();
    dlfl::Vertex_p* vs = new dlfl::Vertex_p[2];
    vs[0] = pMS->addMeshVertex(o + n*rad);
    vs[1] = pMS->addMeshVertex(o - n*rad);
    return vs;
}


MeshShape* SpineShape::buildMeshShape(MeshShape* pMS){

    if (!pMS)
        pMS = new MeshShape();

    using namespace dlfl;

    Mesh_p omesh = pMS->mesh();

    int id = 0;
    for(std::list<SVertex_p>::iterator it = _verts.begin(); it != _verts.end(); it++)
        (*it)->id = id++;

    int osz = _verts.size() * _verts.size();
    OutlineMap omap = new Vertex**[osz];
    for(int i = 0; i < osz; i++)
        omap[i] = 0x00;

    for(list<SVertex_p>::iterator it = _verts.begin(); it!= _verts.end();it++){

        //three type verts: Joint (val:3) / Bridge (val:2) / End (val:1)
        SVertex_p v = (*it);
        if (v->val() > 2){ // a joint

            SVertex_p* branches = sortLinks(v); //bok ye

            int sz = v->val();
            Face_p fmid = omesh->addFace(sz);

            for(int i = 0; i < sz; i++){
                Point n = (branches[i]->P() - v->P()).normalize()*0.5 + (branches[(i-1+sz)%sz]->P() - v->P()).normalize()*0.5;
                Vertex_p vn = pMS->addMeshVertex(v->P() + n*RAD*2);//fix it !!
                fmid->set(vn, i);
            }

            fmid->update();

            //now update omesh
            for(int i = 0; i< sz; i++ ){
                Vertex_p v0 = fmid->V(i);
                Vertex_p v1 = fmid->V(i+1);
                if (Vertex_p* vs1 = omap[edgeId(branches[i],v)])
                    omesh->addQuad(v0, vs1[1], vs1[0], v1);
                else{
                    Vertex_p* vs = new Vertex*[2];
                    vs[0] = v0; vs[1] = v1;
                    omap[edgeId(v,branches[i])] = vs;
                }
            }

        }else{ //a bridge or an end vertex
            SVertex_p v0 = ( v->val() == 2 )? v->links.back() : v;
            SVertex_p v1 = v->links.front();
            Point tan = (v1->P() - v0->P()).normalize();
            Vertex** vs = this->getOutlineVerts(v->P(), tan, pMS, RAD);
            if (Vertex** vs1 = omap[edgeId(v1,v)])
                omesh->addQuad(vs[0], vs1[1], vs1[0], vs[1]);
            else
                omap[edgeId(v,v1)] = vs;

            if (v0!=v){ //inverse order in neg tangent direction

                if (Vertex** vs1 = omap[edgeId(v0,v)]){
                    omesh->addQuad(vs[1], vs1[1], vs1[0], vs[0]);
                }else{
                    //Vertex* vs_i[2] = {vs[1], vs[0]}; // this causes error! figure it out!!!!
                    Vertex** vs_i = new Vertex*[2];
                    vs_i[0] = vs[1]; vs_i[1] = vs[0];
                    omap[edgeId(v,v0)] = vs_i;
                }
            }
        }
    }
    omesh->buildEdges();

    if (MeshShape::isSMOOTH)
        pMS->makeSmoothTangents();

    pMS->Renderable::update();
    pMS->pP()->set(P());

    return pMS;
}

SpineShape::SVertex_p* SpineShape::sortLinks(SVertex_p v, SVertex_p v0){//sorts linked verticies with respet to v0

    int sz = v->links.size();
    if (!v0)
        v0 = v->links.front();

    SVertex_p * vs = new SVertex_p[sz];
    Point x = (v0->P() - P()).normalize();
    Point y = (Vec3(x)%Vec3(0,0,1)).normalize();
    vs[0] = v0;
    int k = 1;

    for(std::list<SVertex_p>::iterator it = v->links.begin(); it != v->links.end(); it++)
        if (*it != v0 )
            vs[k++] = (*it);

    double * deg = new double[sz];
    for(int i = 0; i<sz; i++ ){
        Point n((vs[i]->P() - P()).normalize());
        deg[i] = (n * x);
        if (n*y<0)
            deg[i] = -2 - deg[i];
    }

    //now sort:
    for(int i = 1; i<sz; i++)
        for(int j=i+1; j<sz; j++)
            if (deg[i] < deg[j]){
                //swap
                double d = deg[j];
                deg[j] = deg[i];
                deg[i] = d;
                SVertex_p tmp = vs[j];
                vs[j] = vs[i];
                vs[i] = tmp;
            }

    return vs;
}
