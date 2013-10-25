#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include "base.h"

#define NORMAL_RAD 0.04
#define NORMAL_CONTROL_BIT 30
#define SV_ID_BIT 16

class Shape;
typedef Shape*                          Shape_p;
typedef std::list<Shape_p>              ShapeList;

struct  ShapeVertex;
typedef ShapeVertex*                    ShapeVertex_p;
typedef std::list<ShapeVertex_p>        SVList;
typedef std::map<int, ShapeVertex_p>    SVMap;

struct BBox{
    Point P[2];

    Vec2        diag() const  {return P[1] - P[0];}
    Point       pivot() const {return (P[0]+P[1])*0.5;}
};

struct ShapeVertex {

    Point           P;
    Normal          N;
    unsigned long   flag;
    void*           pRef;
    bool            isPositionControl, isNormalControl;

    //enum SVType     {POINT, TANGENT};

    void drag(const Vec2& t, bool isNormal = false);

    void adopt(ShapeVertex_p sv);
    void setPair(ShapeVertex_p sv, bool isSetTangent =false , bool isSetNormal = false);
    void unpair();

    inline Point_p       pP()                 {return &P;}
    inline Normal_p      pN()                 {return &N;}
    inline Shape_p       shape()        const {return _pShape;}

    inline ShapeVertex_p parent()       const {return _parent;}
    inline ShapeVertex_p pair()         const {return _pair;}
    inline int           id()           const {return _id;}
    inline bool          hasChilds()    const {return _childs.size()>0;}
    inline SVList        getChilds()    const {return _childs;}


    void update();
    static ShapeVertex_p get(int id);

private:

    friend class Shape;

    int                 _id;
    Shape_p             _pShape;
    bool                _isPositionControl, _isNormalControl;

    ShapeVertex(Shape_p pS);
    ~ShapeVertex();

    ShapeVertex_p _parent, _pair;
    SVList _childs;

    static int _COUNT;
    static SVMap _svmap;
};

class Shape:public Draggable{

    Point                   _t0;
    Matrix3x3               _tM; //the transform matrix
    Point                   _piv;

    unsigned int            _flags;
    SVList                  _vertices;

protected:

    virtual void onDrag(const Vec2&){}
    virtual void onRotate(double ang){}
    virtual void onScale(const Vec2&){}
    virtual void onApplyT(const Matrix3x3&){}

    virtual void onClick(const Point&, Click_e){}

public:

    Shape();
    virtual ~Shape();
    void                renderAll() const;

    //Vertex Handling
    ShapeVertex_p       addVertex();
    ShapeVertex_p       addVertex(const Point& p, ShapeVertex_p parent = 0, bool isPositionControl = true, bool isNormalControl = true);
    void                removeVertex(ShapeVertex_p sv);
    void                removeVertex(Point_p pP);
    SVList              getVertices() const {return _vertices;}
    void                update(ShapeVertex_p sv);

	//send generic command to the shape
    enum    Command_e {};
    virtual void        exec(Command_e){}
    void                sendClick(const Point& p, Click_e click);

    //transformations
    Point               gT();
    inline Matrix3x3    getTransform(){return _tM;}
    inline Matrix3x3*   pTransform(){return &_tM;}
    void                translate(const Vec2&);
    void                rotate(double ang);
    void                scale(const Vec2&);
    void                resetT();
    void                frezeT();
    void                applyT(const Matrix3x3&);

    virtual void        getBBox(BBox& bbox) const;
    void                centerPivot();

	//flag operations
    bool                is(unsigned int bit){return _flags & (1 << bit);}
    void                set(unsigned int bit){_flags |= (1 << bit);}
    void                unset(unsigned int bit){_flags &= ~(1 << bit);}

	//save&load
    virtual int         load(std::ifstream&){return -1;}
    virtual int         save(std::ofstream&){return -1;}

};

#endif
