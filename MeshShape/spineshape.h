#ifndef SPINESHAPE_H
#define SPINESHAPE_H

#include "../shape.h"
#include "CMesh.h"

class SpineShape : public Shape
{

    struct SVertex;
    typedef std::list<SVertex*> SVertexList;
    typedef struct SVertex{

        Point_p pP;
        int id;
        SVertexList links;
        inline Point P() const {return *pP;}

        inline int val(){return links.size();}

    }* SVertex_p;

    SVertexList _verts;
    SVertex_p _lastV;

    SVertex_p* sortLinks(SVertex_p v, SVertex_p v0 = 0);
    int edgeId(SVertex_p v0, SVertex_p v1){  return v0->id + v1->id*_verts.size();}
    int edgeUId(SVertex_p v0, SVertex_p v1){ return ((v0->id < v1->id)?v0->id:v1->id) + ((v0->id > v1->id)?v0->id:v1->id)*_verts.size();}
    dlfl::Vertex_p* getOutlineVerts(const Point& o, const Point& tan, dlfl::Mesh_p m, double rad, bool control = true);

protected:

    void onClick(const Point &, Click_e);

public:

    SpineShape(){ _lastV=0; }
    void render() const;

    dlfl::Mesh_p buildOutline();

    static double RAD;

};

#endif // SPINESHAPE_H
