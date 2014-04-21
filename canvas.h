#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <Qlist>
#include <Qhash>
#include <QImage>
#include <algorithm>
#include "Shape.h"

#define UNITZ 0.01
#define MAX_LIGHTS 8

#ifndef MODELING_MODE
void updateGLSLLight(double, double, double);


typedef class Light:public Draggable
{
    Point _p0;

protected:

    void onDrag(const Vec2& t, int button = 0){
       if(button != 0)
           _z+= 2*(t.x+t.y);
       updateGLSLLight(_p0.x, _p0.y, _z);
    }

public:

    Light(float x, float y, float z):Draggable(true, &_p0){
        _p0.set(x,y);
        _z = z;
    }
    float _z;
    void render(int mode = 0);

} * Light_p;

#endif


class ImagePlane
{
    unsigned int    _tex;
    QImage          _img;
    double          _aspect;

public:

    ImagePlane();

    bool            readFromFile(const string &fname);
    QImage*         image() {return &_img;}
    QRgb            getColor(const Point& p) const;

    bool            hasTexture() const;

    void            render();

};

class Canvas{

    ShapeList       _shapes;
    ImagePlane*     _pBGImage;
    friend class    GLWidget;

public:

    void            insert(Shape_p sp);
    void            remove(Shape_p sp);

    Shape_p         findPrev(Shape_p);
    Shape_p         findNext(Shape_p);

    void            moveDown(Shape_p);
    void            moveUp(Shape_p);
    void            sendToBack(Shape_p);
    void            sendToFront(Shape_p);
    void            updateDepth();
    void            clear();

    int             getNumShapes(){return _shapes.size();}
    ShapeList       shapes(){return _shapes;}
    void            setShapeList(ShapeList& shapes){ _shapes = shapes;}

    int             saveTo(const char * fname);
    int             loadFrom(const char * fname);

    void            setImagePlane(const string &filename);
    ImagePlane*     bgImage()const {return _pBGImage;}


#ifndef MODELING_MODE

    Point lightPos(int i)const {return _lights[i]->P();}
    Canvas(){
        _lights[0]  = new Light(-0.5, 0.5, 1.0);
        _pBGImage   =  new ImagePlane();
    }
private:
    Light_p         _lights[MAX_LIGHTS];

#else
    Canvas(){
        _pBGImage   =  new ImagePlane();
    }
#endif



};

#endif
