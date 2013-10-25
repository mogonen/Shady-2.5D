#ifndef CONTROLPOINT_H
#define CONTROLPOINT_H

#include "base.h"

class ControlPoint;

typedef ControlPoint* ControlPoint_p;
typedef std::list<ControlPoint_p> CPList;

class ControlPoint: public Draggable{

    static ControlPoint_p _pTheActive;

protected:

    Selectable_p _pControlled;

    void onDown(){
        if (isChild())
            _pTheActive = (ControlPoint_p)parent();
        else
            _pTheActive = this;
    }

    virtual void onDrag(const Point& t){ // move the children
        if (isParent()){
            DraggableList childs = getChilds();
            for(DraggableList::iterator it = childs.begin(); it!= childs.end(); it++)
                (*it)->pP()->set((*it)->P() + t);
        }

        if (_pControlled)
            _pControlled->update();
    }

public:

    ControlPoint(Point_p pP, Selectable_p pControlled):Draggable(Renderable::UI, pP), _pControlled(pControlled){}

    void render() const;
    bool isActive() const {return (_pTheActive == this) || (_pTheActive == this->parent());}
};

#endif // CONTROLPOINT_H
