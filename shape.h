#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <QColor>
#include "controlpoint.h"
#include "Renderer/shaderparameters.h"

#define NORMAL_RAD 0.04

enum  ShapeType {SHAPE_VERTEX, MESH_SHAPE, SPINE_SHAPE, ELLIPSE_SHAPE, IMAGE_SHAPE};

class ShapeBase;
class Shape;
class Shader;
class LayerNormalControl;

typedef Shape*                          Shape_p;
typedef std::list<Shape_p>              ShapeList;

class   ShapeVertex;
class   ControlNormal;
typedef ShapeVertex*                    ShapeVertex_p;
typedef std::list<ShapeVertex_p>        SVList;
typedef std::map<int, ShapeVertex_p>    SVMap;

//4 channel unsigned bytes assumed here.
struct textureInfo
{
    unsigned char *data;
    int width;
    int height;
    textureInfo():data(NULL), width(0), height(0)
    {}
};

struct BBox{
    Point P[2];
    Vec2                diag() const  {return P[1] - P[0];}
    Point               pivot() const {return (P[0]+P[1])*0.5;}
};


class SVCache{

    ShapeVertex_p _sv, _pair, _parent;
    DraggableList _childs;

    Point  _P;
    Normal _N;

public:
    SVCache(ShapeVertex_p sv);
    void restore();
};


typedef class ShapeBase {

    ControlNormal*       _pControlN;
    bool                 isN;

    friend class SBCache;

public:

    ShapeBase(bool isN = false);

    void                 dragNormal(const Vec2 &t);
    virtual void         outdate(){}
    RGB                  value[ACTIVE_CHANNELS];
    Point                _P; //this could be a channel too?


    inline Normal        N()    const {return  value[NORMAL_CHANNEL];}  //_N;}
    inline Normal_p      pN()   {return &value[NORMAL_CHANNEL];} //_N;}//;}

    ControlNormal*       pControlN()    const {return _pControlN;}
    virtual              ShapeType      type() const = 0;

} * ShapeBase_p;

class SBCache{

    ShapeBase_p         _pSB;
    RGB                 _value[ACTIVE_CHANNELS];
    Point               _p;

public:

    void set(ShapeBase_p pSB);
    void restore();

};

class ShapeVertex:public ControlPoint, public ShapeBase
{

protected:

    void onDrag(const Point &, int button);

public:

    unsigned long       flag;
    bool                isPositionControl, isNormalControl;

    void                 setPair(ShapeVertex_p sv, bool isSetTangent =false , bool isSetNormal = false);
    void                 unpair();
    void                 setTangent(const Vec2&, bool isnormal=false, bool ispair=false);
    Vec2                 getTangent();
    ShapeType            type() const {return SHAPE_VERTEX;}


    inline Shape_p       shape()        const {return _pShape;}


    inline ShapeVertex_p pair()         const {return _pair;}
    inline int           id()           const {return name();}
    inline bool          isDeleted()    const {return _isDeleted;}

    Point                gP();
    void                 setGlobalP(const Point&);

    void                 outdate();
    bool                _isDeleted; //public for now

private:

    friend class Shape;
    friend class SVCache;

    Shape_p             _pShape;
    //Point               _P;
    //Normal            _N;

    ControlNormal*      _pControlN;

    ShapeVertex(Shape_p pS, bool isP = true, bool isN = true);
    ~ShapeVertex();

    ShapeVertex_p _pair;
    //color
    //preview implementation
};

class ControlNormal:public ControlPoint
{

    //ShapeVertex_p   _pSV;
    ShapeBase_p       _pShapeBase;

protected:
    void onDrag(const Point &t, int button);

public:

    ControlNormal(ShapeBase_p pSB):ControlPoint(0){
        _color[0] = 1.0;
        _pShapeBase = pSB;
        makeDraggable();
    }

    inline Point P()    const;

};


class Shape:public Draggable, public ShapeBase{

    //Point                _t0;
    Matrix3x3            _tM; //the transform matrix
    Point                _piv;

    unsigned int         _flags;
    SVList               _vertices;
    //Shader*            _pShader;

protected:
    virtual void        onDrag(const Vec2&){}
    virtual void        onRotate(double ang){}
    virtual void        onScale(const Vec2&){}
    virtual void        onApplyT(const Matrix3x3&){}
    virtual void        onClick(const Click&){}

    virtual void        onStartDrag(ShapeVertex_p){}
    virtual void        onStopDrag(ShapeVertex_p){}
    virtual void        onDrag(ShapeVertex_p, const Vec2&){}

public:

    Shape();
    virtual ~Shape();
    void                render(int mode = 0);


    //Vertex Handling
    ShapeVertex_p       addVertex();
    ShapeVertex_p       addVertex(const Point& p, ShapeVertex_p parent = 0, bool isPositionControl = true, bool isNormalControl = true);
    void                removeVertex(ShapeVertex_p sv);
    //void                removeVertex(Point_p pP);
    SVList              getVertices() const {return _vertices;}

    void                outdate(){Renderable::outdate();}
    virtual void        outdate(ShapeVertex_p sv){ Renderable::outdate(); }

	//send generic command to the shape
    enum    Command_e {};
    virtual void        exec(Command_e){}
    void                sendClick(const Click&);

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
    void                drag(ShapeVertex_p, const Vec2&);

    virtual void        getBBox(BBox& bbox) const;
    void                centerPivot();

	//flag operations
    bool                is(unsigned int bit){return _flags & (1 << bit);}
    void                set(unsigned int bit){_flags |= (1 << bit);}
    void                unset(unsigned int bit){_flags &= ~(1 << bit);}

    //for now
    //QColor diffuse;


#ifndef MODELING_MODE
    //These eventually need to move out
    //Shader related funcs --> actually should not be here
    virtual void        calAverageNormal(){_shaderParam.m_averageNormal = QVector2D(0.0,0.0);}
    ShaderParameters    getShaderParam(){return _shaderParam;}
    void                setLayerLabel(unsigned char dep = 0){_shaderParam.m_layerLabel = dep;}

    textureInfo         m_brightTex;
    textureInfo         m_darkTex;
    textureInfo         m_smTex;

    //protected:
    ShaderParameters         _shaderParam;


#endif

};

#endif

