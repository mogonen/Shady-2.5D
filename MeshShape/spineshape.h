#ifndef SPINESHAPE_H
#define SPINESHAPE_H

#include "meshshape.h"

class SpineShape : public Shape
{

    struct SVertex;
    typedef std::list<SVertex*> SVertexList;
    typedef struct SVertex{

        ShapeVertex_p pSV;
        int id;
        SVertexList links;
        inline Point P() const {return pSV->P();}
        inline int val(){return links.size();}

        SVertex(){
            pSV = 0;
            id = -1;
        }

    }* SVertex_p;

    SVertexList _verts;
    SVertex_p _lastV;

    SVertex_p* sortLinks(SVertex_p v, SVertex_p v0 = 0);
    int edgeId(SVertex_p v0, SVertex_p v1){  return v0->id + v1->id*_verts.size();}
    int edgeUId(SVertex_p v0, SVertex_p v1){ return ((v0->id < v1->id)?v0->id:v1->id) + ((v0->id > v1->id)?v0->id:v1->id)*_verts.size();}
    dlfl::Vertex_p* getOutlineVerts(const Point& o, const Point& tan, MeshShape* pMS, double rad);

protected:

    void onClick(const Click&);

public:

    SpineShape(){ _lastV=0; }
    void render(int mode = 0);
    ShapeType   type() const {return SPINE_SHAPE;}

    MeshShape*      buildMeshShape(MeshShape* pMS=0);

    static double RAD;

};

#endif // SPINESHAPE_H
