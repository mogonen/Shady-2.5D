#ifndef MESHCOMMANDS_H
#define MESHCOMMANDS_H

#include "../base.h"
#include "meshshape.h"

using namespace dlfl;

class MeshOperation: public Command
{

    Face_p      _pF;
    Edge_p      _pE;
    MeshShape*  _pMS;

protected:

    void onClick(const Click &);

    void                    insertSegment(Edge_p, const Point&);
    void                    diagonalDivide(Corner_p);
    Face_p                  extrude(Face_p, double);
    Edge_p                  extrude(Edge_p, double, VertexMap* vmap=0);
    void                    deleteFace(Face_p);

public:

    enum OperationMode  {NONE, EXTRUDE_EDGE, EXTRUDE_FACE, DELETE_FACE, SPLIT_FACE, INSERT_SEGMENT,  ASSIGN_PATTERN, SET_FOLDS, ENDOP};
    enum SelectMode     {NOSELECT, EDGE, FACE, CORNER, EDGE_EDGE};


    MeshOperation(OperationMode op):Command(){
        _operation = op;
    }

    SelectMode getSelectMode()
    {
        static const SelectMode MODES[] = {NOSELECT, EDGE, FACE, FACE, CORNER, EDGE, EDGE, EDGE, NOSELECT};
        if (_operation >= ENDOP)
            return NOSELECT;
        return MODES[(int)_operation];
    }

    CommandType type() const {return MESH_OPERATION;}


    Command_p exec();
    Command_p unexec();


    static int              FOLD_N;
    static double           FOLD_D;

    static bool             EXEC_ONCLICK;
    static double           EXTRUDE_T;
    static bool             isKEEP_TOGETHER;

    static string           PATTERN;

private:

    OperationMode           _operation;
    Click                   _click;
    void                    execOP(Selectable_p);


};

class MeshPrimitive: public Command
{

protected:

    void onClick(const Click &);

public:

    enum Primitive {GRID, TWO_NGON, TORUS, SPINE};

    MeshPrimitive(Primitive p){
        _primitive = p;
    }

    CommandType type() const {return MESH_PRIMITIVE;}

    Command_p exec();
    Command_p unexec();


    static MeshShape*       insertGrid(const Point& p, double nlen, double mlen, int n, int m, MeshShape* pMS = 0);
    static MeshShape*       insertNGon(const Point& p, int n, int segv, double rad, MeshShape* pMS = 0);
    static MeshShape*       insertTorus(const Point& p, int n, int v, double rad, double w, double arc, MeshShape* pMS = 0);

    static double           GRID_N_LEN;
    static double           GRID_M_LEN;
    static int              GRID_N;
    static int              GRID_M;

    static int              NGON_N;
    static int              NGON_SEG_V;
    static double           NGON_RAD;

    static int              TORUS_N;
    static int              TORUS_V;
    static double           TORUS_RAD;
    static double           TORUS_W;
    static double           TORUS_ARC;

private:

    Primitive _primitive;
};


#endif // MESHCOMMANDS_H
