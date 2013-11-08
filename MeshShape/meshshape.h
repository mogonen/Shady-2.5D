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
    void onEnsureUpToDate();
    //void onOutdate();


    void render(Edge_p) const;
    void render(Face_p) const;

    Vertex_p addMeshVertex();
    Vertex_p addMeshVertex(const Point&);

    friend class SpineShape;

public:

    MeshShape();
    Mesh_p                  mesh() const {return _control;}
    void                    outdate(ShapeVertex_p sv);

    //primitives
    static MeshShape*       insertGrid(const Point& p, double len, int n, int m, MeshShape* pMS = 0);
    static MeshShape*       insertNGon(const Point& p, int n, int segv, double rad, MeshShape* pMS = 0);
    static MeshShape*       insertTorus(const Point& p, int n, double rad, MeshShape* pMS = 0);


    enum OPERATION_e        {NONE, EXTRUDE_EDGE, EXTRUDE_FACE, DELETE_FACE, SPLIT_FACE,
                             INSERT_SEGMENT, INSERT_GRID, INSERT_2NGON, INSERT_TORUS, INSERT_SPINE};

    enum SELECTION_e        {NOSELECT, EDGE, FACE, CORNER, EDGE_EDGE};

    static void             setOPMODE(OPERATION_e eMode);
    static SELECTION_e      GetSelectMode();
    inline static bool      IsSelectMode(SELECTION_e eMode);
    void                    makeSmoothTangents();
    void                    makeSmoothTangents(Corner_p);

    static void             executeStackOperation();
    static void             execOP(const Point&, Selectable_p obj);

    static double           GRID_LEN;
    static int              GRID_N;
    static int              GRID_M;
    static int              NGON_N;
    static int              NGON_SEG_V;
    static double           NGON_RAD;
    static int              TORUS_N;
    static double           TORUS_RAD;
    static bool             EXEC_ONCLICK;
    static bool             isSMOOTH;
    static double           EXTRUDE_T;

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
    void                    adjustInsertedSegmentTangents(Corner_p);

};

//Callbacks
void                        onInsertEdge(Edge_p);
void                        onAddFace(Face_p);
Bezier*                     initCurve(Edge_p);


//void                        executeMeshShapeOperation();



#endif // MESHSHAPE_H
