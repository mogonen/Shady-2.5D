#ifndef MESHSHAPE_H
#define MESHSHAPE_H

#include "../Shape.h"
#include "CMesh.h"
#include "MeshData.h"

using namespace dlfl;

class MeshShape : public Shape
{

protected:
    Mesh_p _control;

    //overridden functions
    void render() const;
    void onClick(const Point &, Click_e);
    void onUpdate();
    //void onOutdate();


    void render(Edge_p) const;
    void render(Face_p) const;
    void execOP(const Point&);

    Vertex_p addMeshVertex();
    Vertex_p addMeshVertex(const Point&);

public:

    MeshShape(Mesh_p control = 0);

    //primitives
    static MeshShape*       insertGrid(const Point& p, double len, int n, int m, MeshShape* pMS = 0);
    static MeshShape*       insertNGon(const Point& p, int n, double rad, MeshShape* pMS = 0);

    static bool             isSMOOTH;
    static double           EXTRUDE_T;

    enum OPERATION_e        {NONE, EXTRUDE_EDGE, EXTRUDE_FACE, DELETE_FACE, SPLIT_FACE, INSERT_SEGMENT};
    enum SELECTION_e        {NOSELECT, EDGE, FACE, CORNER, EDGE_EDGE};
    enum PRIMITIVE_e        {SQUARE, GRID, NGON, SPINE};

    static void             setOPMODE(OPERATION_e eMode);
    static SELECTION_e      GetSelectMode();
    inline static bool      IsSelectMode(SELECTION_e eMode);
    void                    makeSmoothTangents();

private:

    static OPERATION_e _OPMODE;

    //mesh operations
    void                    insertSegment(Edge_p, const Point&);
    void                    diagonalDivide(Corner_p);
    Face_p                  extrude(Face_p, double);
    Edge_p                  extrude(Edge_p, double);
    void                    deleteFace(Face_p);

    //helper functions
    void                    onSplitEdge(Corner_p, double t);

};

//Callbacks
void                        onInsertEdge(Edge_p);
void                        onAddFace(Face_p);
Bezier*                     initCurve(Edge_p);


#endif // MESHSHAPE_H
