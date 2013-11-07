#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <Qlist>
#include <Qhash>
#include <algorithm>
#include "Shape.h"

#define UNITZ 0.01
#define MAX_LIGHTS 8

void updateGLSLLight(double, double, double);

typedef class Light:public Draggable
{
    Point _p0;
    float _z;
protected:

    void onDrag(const Vec2& t){
       _z+= 2*(t.x-t.y);
       updateGLSLLight(_p0.x, _p0.y, _z);
    }

public:

    Light(float x, float y, float z):Draggable(UI, &_p0){
        _p0.set(x,y);
        _z = z;
    }

    void render() const;

} * Light_p;

class Canvas{

    ShapeList       _shapes;
    Light_p         _lights[MAX_LIGHTS];
    friend class    GLWidget;

public:

    Canvas(){
        _lights[0] = new Light(-0.5, 0.5, 1.0);
    }

    void insert(Shape_p sp);
    void remove(Shape_p sp);

    Shape_p findPrev(Shape_p);
    Shape_p findNext(Shape_p);

    void moveDown(Shape_p);
    void moveUp(Shape_p);
    void sendToBack(Shape_p);
    void sendToFront(Shape_p);
    void clear();

	int saveTo(const char * fname);
	int loadFrom(const char * fname);

    void setImagePlane(const string &filename);
    Point lightPos(int i)const {return _lights[i]->P();}

};

#endif