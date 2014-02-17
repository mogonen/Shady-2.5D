#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "base.h"

class ControlPoint;

typedef ControlPoint* ControlPoint_p;
typedef std::list<ControlPoint_p> CPList;

class ControlPoint: public Draggable{

    static ControlPoint_p _pTheActive;

protected:

    Renderable_p _pControlled;
    float        _color[3];

    void onDown(){
        if (isChild())
            _pTheActive = (ControlPoint_p)parent();
        else
            _pTheActive = this;
    }

    virtual void onDrag(const Point& t, int button){ // move the children
        Draggable::onDrag(t, button);
        if (_pControlled)
            _pControlled->update();
    }

public:

    ControlPoint(Point_p pP, Renderable_p pControlled=0):Draggable(true, pP), _pControlled(pControlled){
        _color[0] = _color[1] = _color[2] = 0.0;
    }

    void render(int mode = 0);
    bool isActive() const {return (_pTheActive == this) || (_pTheActive == this->parent());}
};

#endif // CONTROLPOINT_H
