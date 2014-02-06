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

    void drag(const Vec2& t, bool isNormal = false);

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

    void                 outdate();
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
    virtual void        onClick(const Point&, Click_e){}
    ShaderParameters       _shaderParam;

public:

    Shape();
    virtual ~Shape();
    void                render(int mode = 0);

    //Vertex Handling
    ShapeVertex_p       addVertex();
    ShapeVertex_p       addVertex(const Point& p, ShapeVertex_p parent = 0, bool isPositionControl = true, bool isNormalControl = true);
    void                removeVertex(ShapeVertex_p sv);
    void                removeVertex(Point_p pP);
    SVList              getVertices() const {return _vertices;}
    virtual void        outdate(ShapeVertex_p sv){ Renderable::outdate(); }

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



    //Shader related funcs --> actually should not be here
    virtual void        calAverageNormal(){_shaderParam.m_averageNormal = QVector2D(0.0,0.0);}
    ShaderParameters    getShaderParam(){return _shaderParam;}
    void                setLayerLabel(unsigned char dep = 0){_shaderParam.m_layerLabel = dep;}



    textureInfo m_brightTex;
    textureInfo m_darkTex;
    textureInfo m_smTex;

	//save&load
    virtual int         load(std::ifstream&){return -1;}
    virtual int         save(std::ofstream&){return -1;}

    //for now
    QColor diffuse;

};

#endif


/*
//    ShaderParameters* shader() const {return _shaderParam;}
//    ShaderParameters* initializeParam()
//        if(_shaderParam)
//            delete _shaderParam;
//        _shaderParam = new ShaderParameters();
//        return _shaderParam;
//    }

//    void setAlpha()
//    {

//    }

//    void setTranslucency()
//    {

//    }

//    void setSMQuality()
//    {

//    }

//    void toggleMirror()
//    {

//    }

//    void setBrightParam()
//    {
////        if(m_brightTex.data)
////            _shaderParam->LoadBrightImage(m_brightTex.data,m_brightTex.width,m_brightTex.height);
//    }
//    void setDarkParam()
//    {
////        if(m_darkTex.data)
////            _shaderParam->LoadDarkImage(m_darkTex.data,m_darkTex.width,m_darkTex.height);
//    }

//    void setShapeMapTex()
//    {

//    }

//    virtual void loadBrightTex(QString name = QString())
//    {
////        QImage m_temp(name);
////        m_brightTex
//    }
//    virtual void loadDarkTex(QString name)
//    {

//    }
//    virtual void loadSMTex(QString name){

//    }
*/
