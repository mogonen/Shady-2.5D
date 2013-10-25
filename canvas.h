#ifndef __CANVAS_H__
#define __CANVAS_H__

#include <list>
#include <hash_set>
#include <algorithm>
#include "Shape.h"

#define UNITZ 0.01

typedef class Light:public Draggable
{
public:
    Light(const Point& p):Draggable(UI, new Point(p)){}
    void render() const;

} * Light_p;

class Canvas:public Renderable{

    ShapeList _shapes;
    Shape_p _active;

    Light_p _lights[4];

    static Canvas* _canvas;
    static double _tM[16];

public:

    Canvas():Renderable(NONE){
       _active = 0;
       isDragMode = true;
       isShadingOn = false;

       isNormalsOn   = false;
       isWireframeOn = false;
    }

    void setImagePlane(const string &filename);
    void render() const;
    void render(Shape_p) const;

    void insert(Shape_p sp){
        _shapes.push_front(sp);
		activate(sp); 
    }

    void remove(Shape_p sp){
        _shapes.remove(sp);
		_shapes.remove(sp);
		delete sp;
    }

    Shape_p removeActive(){
        if (!_active)
			return 0;
		remove(_active);
        Shape_p tmp = _active;
		_active = 0;
		return tmp;
    }

    void activate(Shape_p sp){
		if (!sp)
			return;
		_active = sp;
        //_active->onActivate();
    }

	void deactivate(){
		if (!_active)
			return;
        //_active->onDeactivate();
        _active = 0;
    }

	void selectPrev(){
		if (!_shapes.size())
			return;
		if (_shapes.size() == 1){
			activate(_shapes.front());
			return;
		}
        ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), _active);
		if (it == _shapes.begin())
			it = _shapes.end();
		it--;
		activate(*it);
    }

	void selectNext(){
		if (!_shapes.size())
			return;
		if (_shapes.size() == 1){
			activate(_shapes.front());
			return;
		}
        ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), _active);
		if (it != _shapes.end())
			it++;
		
		if (it == _shapes.end())
			it = _shapes.begin();
	
		activate(*it);
    }

    void activeDown(){

		if (!_active)
			return;

        ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), _active);
        ShapeList::iterator itn = it;
		itn++;
		if ( itn == _shapes.end())
			return; 
        Shape_p tmp = *it;
		(*it) = *itn;
		(*itn) = tmp;
	}

    void activeUp(){

		if (!_active)
			return;

        ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), _active);
        ShapeList::iterator itp = it;
		if ( itp == _shapes.begin())
			return; 
		itp--;
        Shape_p tmp = *it;
		(*it) = *itp;
		(*itp) = tmp;
    }

    void sendToBack(){
        if (!_active)
            return;
        _shapes.remove(_active);
        _shapes.push_back(_active);
    }

    void sendToFront(){
        if (!_active)
            return;
        _shapes.remove(_active);
        _shapes.push_front(_active);
    }

	void clear(){
        FOR_ALL_ITEMS(ShapeList,_shapes)
			delete *it;
		_shapes.clear();
        _active = 0;
    }

    Shape_p active(){return _active;}

	int saveTo(const char * fname);
	int loadFrom(const char * fname);


    //lights
    void initLights(){
        _lights[0] = new Light(Point(-0.75, 0.75));
    }
    Point lightPos(int i)const {return _lights[i]->P();}

    bool isDragMode;
    bool isShadingOn;

    bool isNormalsOn;
    bool isWireframeOn;

    bool canDragShape(){ return isDragMode && (MODE == POINT_NORMAL_SHAPE_M || MODE == SHAPE_M); }

    void flipMode(){isDragMode=!isDragMode;}

	//statics
    static Canvas* get(){return _canvas;}
    enum EditMode_e {POINT_NORMAL_SHAPE_M, POINT_NORMAL_M, POINT_M, NORMAL_M, SHAPE_M, SHADED_M, TEXTURED_M, VIEW_M};
    static EditMode_e MODE;

    static void apply(const Matrix3x3& tM){
        _tM[0] = tM[0].x;
        _tM[1] = tM[1].x;
        _tM[2] = 0;
        _tM[3] = 0;

        _tM[4] = tM[0].y;
        _tM[5] = tM[1].y;
        _tM[6] = 0;
        _tM[7] = 0;

        _tM[8]  = 0;
        _tM[9]  = 0;
        _tM[10] = 1;
        _tM[11] = 0;

        _tM[12] = tM[0].z;
        _tM[13] = tM[1].z;
        _tM[14] = 0;
        _tM[15] = 1;
    }
};

#endif
