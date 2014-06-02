#ifndef MESHSHAPE_H
#define MESHSHAPE_H

#include <string>
#include "../Shape.h"
#include "CMesh.h"
#include "patch.h"

//#include "MeshData.h"

using namespace dlfl;

typedef std::map<Vertex_p, Corner_p> VertexMap;


class MeshShape : public Shape
{
protected:
    Mesh_p _control;

    //overridden functions
    void                    render(int mode = 0);
    void                    onUpdate();
    void                    onEnsureUpToDate();
    void                    onDrag(ShapeVertex_p, const Vec2&);

    //void onOutdate();
    friend class            SpineShape;
    friend class            MeshOperation;
    friend class            MeshPrimitive;

public:

    MeshShape();
    Mesh_p                  mesh() const {return _control;}
    ShapeType               type() const {return MESH_SHAPE;}
    void                    outdate(ShapeVertex_p sv);

    void                    set(Mesh_p);

    Vertex_p                addMeshVertex(ShapeVertex_p sv =0);
    Vertex_p                addMeshVertex(const Point&);

    void                    makeSmoothTangents(bool isskip = false, int ttype = 2, double tank =1.0);
    static void             makeSmoothTangents(Corner_p);
    static void             makeSmoothCorners(Corner_p, bool isskipsharp = true, int tangenttype=1, double tan_k=1.0);

    //mesh operations
    static bool             isSMOOTH;
    static Patch::PatchType PATCH_TYPE;

    void                    addSurface(Face_p);
    Patch::PatchType        _patchtype;

    void test();
};

    //Callbacks
    void                    onInsertEdge(Edge_p);
    void                    onUnsplitEdge(Corner_p pC);
    void                    onAddFace(Face_p);
    void                    initCurve(Corner_p);

//helper functions

//void                        executeMeshShapeOperation();



#endif // MESHSHAPE_H
