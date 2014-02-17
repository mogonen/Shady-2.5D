#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include <QColor>
#include "base.h"
#include "Renderer/shaderparameters.h"

#define NORMAL_RAD 0.04
#define NORMAL_CONTROL_BIT 30
#define SV_ID_BIT 16

enum ShapeType {MESH_SHAPE, SPINE_SHAPE, ELLIPSE_SHAPE, IMAGE_SHAPE};

class Shape;
class Shader;
typedef Shape*                          Shape_p;
typedef std::list<Shape_p>              ShapeList;

struct  ShapeVertex;
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

struct ShapeVertex
{
    Point               P;
    Normal              N;
    unsigned long       flag;
    void*               pRef;
    bool                isPositionControl, isNormalControl;

    QColor              C0, C1; //dark & bright images -> later should be moved to somewhere else

    void drag(const Vec2& t, bool isNormal = false, bool isC2 = false);

    void adopt(ShapeVertex_p sv);
    void setPair(ShapeVertex_p sv, bool isSetTangent =false , bool isSetNormal = false);
    void unpair();
    void setTangent(const Vec2&, bool isnormal=false, bool ispair=false);
    Vec2 getTangent();

    inline Point_p       pP()                 {return &P;}
    inline Normal_p      pN()                 {return &N;}
    inline Shape_p       shape()        const {return _pShape;}

    inline ShapeVertex_p parent()       const {return _parent;}
    inline ShapeVertex_p pair()         const {return _pair;}
    inline int           id()           const {return _id;}
    inline bool          hasChilds()    const {return _childs.size()>0;}
    inline SVList        getChilds()    const {return _childs;}
    inline bool          isDeleted()    const {return _isDeleted;}

    void                 outdate();
    static ShapeVertex_p get(int id);

    bool                _isDeleted; //public for now

private:

    friend class Shape;

    int                 _id;
    Shape_p             _pShape;


    ShapeVertex(Shape_p pS);
    ~ShapeVertex();

    ShapeVertex_p _parent, _pair;
    SVList _childs;

    static int _COUNT;
    static SVMap _svmap;
};

class Shape:public Draggable{

    Point                _t0;
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
    virtual ShapeType   type() const = 0;

    //Vertex Handling
    ShapeVertex_p       addVertex();
    ShapeVertex_p       addVertex(const Point& p, ShapeVertex_p parent = 0, bool isPositionControl = true, bool isNormalControl = true);
    void                removeVertex(ShapeVertex_p sv);
    //void                removeVertex(Point_p pP);
    SVList              getVertices() const {return _vertices;}

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
    QColor diffuse;

#ifndef MODELING_MODE
    //These eventually need to move out
    //Shader related funcs --> actually should not be here
    virtual void        calAverageNormal(){_shaderParam.m_averageNormal = QVector2D(0.0,0.0);}
    ShaderParameters    getShaderParam(){return _shaderParam;}
    void                setLayerLabel(unsigned char dep = 0){_shaderParam.m_layerLabel = dep;}

    textureInfo m_brightTex;
    textureInfo m_darkTex;
    textureInfo m_smTex;

protected:
    ShaderParameters       _shaderParam;
#endif

};

#endif

