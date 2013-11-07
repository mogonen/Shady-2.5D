#ifndef SHAPECONTROL_H
#define SHAPECONTROL_H

#include "shape.h"
#include "controlpoint.h"
/*
typedef class RotateHandle:public ControlPoint{

    Point _t;

protected:

    void onDrag(Vec2& t){

    }

public:

    void render() const;

}* RotateHandle_p;
*/

class TransformHandler:public Draggable{

    Point           _t, _rot;

    Vec2            _rvec0;

    BBox            _bbox, _bbox_0;
    ControlPoint_p  _handles[2];
    ControlPoint_p  _rotHandle;
    Shape_p         _pShape;

protected:

    void onUpdate(){

        if (!_pShape)
            return;

        //apply the transformation
        Vec2 diag  = _bbox.diag();
        Vec2 diag0 = _bbox_0.diag();
        Vec2 scale(diag.x/diag0.x, diag.y/diag0.y);
        Vec2 t = (_bbox.pivot()-_bbox_0.pivot());

        /*Vec2 rvec1 = (_rot - t).normalize();
        double cosa = _rvec0*rvec1;
        double sina = sqrt(1-cosa*cosa);*/

        _pShape->pTransform()->set(scale.x,0,t.x, 0,scale.y,t.y, 0, 0,1);
        //_pShape->pTransform()->set(scale.x*cosa,-sina,t.x, sina,scale.y*cosa,t.y, 0, 0,1);
        //_rot.set(_bbox.P[1].x, _bbox.P[0].y);
    }

public:

    TransformHandler():Draggable(Renderable::UI, &_t){
        _pShape = 0;
        _handles[0] = new ControlPoint(&_bbox.P[0],this);
        _handles[1] = new ControlPoint(&_bbox.P[1],this);
       // _rotHandle  = new ControlPoint(&_rot, this);
    }

    void setShape(Shape_p pShape){
        _pShape = pShape;
        _pShape->pTransform()->identity();
        _pShape->centerPivot();
        _pShape->getBBox(_bbox_0);
        _bbox = _bbox_0;
        //_rvec0 = (Vec2((_bbox_0.P[1].x, _bbox_0.P[0].y)) - _bbox_0.pivot()).normalize();
        //_rot.set(_bbox.P[1].x, _bbox.P[0].y);
    }

    void apply(){
        if (_pShape){
            _pShape->frezeT();
            //_pShape->pP()->set();
        }
        _pShape = 0;
    }

    void cancel(){
        if (_pShape)
            _pShape->pTransform()->identity();
        _pShape = 0;
    }

    void render() const;

    inline bool isActive() const {return _pShape;}

};

class ShapeControl: public Draggable{

    Point _t0;
    ShapeVertex_p           _theSelected;
    bool                    isNormalControl;

    TransformHandler*       _theHandler;

protected:

    void onDrag(const Vec2& t);

public:

    ShapeControl();

    void startSelect(unsigned int svname);

    void cancel(){
        _theHandler->cancel();
        _theSelected = 0;
    }

    void apply(){
        _theHandler->apply();
    }

    void render() const {}
    void renderControls(const Shape_p);

    void renderHandler() const{
        _theHandler->render();
    }

    void startTransform(Shape_p pS){
        _theHandler->setShape(pS);
    }

};

#endif // SHAPECONTROL_H
