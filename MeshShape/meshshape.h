#ifndef MESHSHAPE_H
#define MESHSHAPE_H

#include <string>
#include "../Shape.h"
#include "CMesh.h"
#include "MeshData.h"

using namespace dlfl;

typedef std::map<Vertex_p, Corner_p> VertexMap;

class MeshShape : public Shape
{

protected:
    Mesh_p _control;

    //overridden functions
    void render(int mode = 0);
    void onUpdate();
    void onEnsureUpToDate();
    void onDrag(ShapeVertex_p, const Vec2&);

    //void onOutdate();

    void render(Edge_p pEdge) const;
    void render(Face_p pFace, int mode = 0) const;

    Vertex_p                addMeshVertex();
    Vertex_p                addMeshVertex(const Point&);



    friend class SpineShape;

public:

    MeshShape();
    Mesh_p                  mesh() const {return _control;}
    ShapeType               type() const {return MESH_SHAPE;}
    void                    outdate(ShapeVertex_p sv);

    //primitives
    static MeshShape*       insertGrid(const Point& p, double nlen, double mlen, int n, int m, MeshShape* pMS = 0);
    static MeshShape*       insertNGon(const Point& p, int n, int segv, double rad, MeshShape* pMS = 0);
    static MeshShape*       insertTorus(const Point& p, int n, int v, double rad, double w, double arc, MeshShape* pMS = 0);

    void                    makeSmoothTangents(bool isskip = false, int ttype = 2, double tank =1.0);
    void                    makeSmoothTangents(Corner_p);
    void                    makeSmoothCorners(Corner_p, bool isskipsharp = true, int tangenttype=1, double tan_k=1.0);

    //mesh operations
    void                    insertSegment(Edge_p, const Point&);
    void                    diagonalDivide(Corner_p);
    Face_p                  extrude(Face_p, double);
    Edge_p                  extrude(Edge_p, double, VertexMap* vmap=0);
    void                    extrudeEdges(SelectionSet, double);
    void                    deleteFace(Face_p);

    void                    assignPattern(Edge_p, string pattern);
    void                    setFolds(Edge_p, int, double min = 0);

    //helper functions
    void                    onSplitEdge(Corner_p, double t);
    void                    adjustInsertedSegmentTangents(Corner_p);

    static bool             isSMOOTH;

};

//Callbacks
void                        onInsertEdge(Edge_p);
void                        onAddFace(Face_p);
Bezier*                     initCurve(Edge_p);
//void                        executeMeshShapeOperation();



#endif // MESHSHAPE_H
