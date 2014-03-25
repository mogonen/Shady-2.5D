#include "commands.h"
#include "canvas.h"

Drag::DragTool     Drag::TOOL = Drag::NONE;
Drag::Continuity   Drag::CONT = Drag::C1;

Drag::Drag(Draggable_p pObj, const Vec2& t){
    _pObj = pObj;
    _t = t;
}

Drag::Drag(SelectionSet set, const Vec2 &t){
    _t = t;
    _selection  = set;
    _pObj = 0;
}

Command_p Drag::exec(){
    return 0;
}

Command_p Drag::unexec(){
    if (_pObj)
        _pObj->drag(-_t);

    if (!_selection.empty()){
        FOR_ALL_ITEMS(SelectionSet, _selection){
            if (!(*it)->isDraggable())
                continue;
            Draggable_p pDragged = (Draggable_p)(*it);
            pDragged->drag(-_t);
        }
    }
    return 0;
}

ShapeOrder::ShapeOrder(Operation op){
    _operation = op;
    _pShape = 0;
}

Command_p ShapeOrder::exec(){

    _shapes = Session::get()->canvas()->shapes();

    if (_pShape==0)
        _pShape = Session::get()->theShape();

    if (_pShape == 0)
        return 0;

switch(_operation){

    case MOVE_UP:
        Session::get()->moveActiveUp();
        break;

    case MOVE_DOWN:
        Session::get()->moveActiveDown();
        break;

    case SEND_BACK:
        Session::get()->sendActiveBack();
        break;

    case SEND_FRONT:
        Session::get()->sendActiveFront();
        break;

    case INSERT_SHAPE:
        Session::get()->insertShape(_pShape);
    break;

    case DELETE_SHAPE:
        Session::get()->removeShape(_pShape);
    break;
}
    return 0;
}

Command_p ShapeOrder::unexec(){        
    Session::get()->canvas()->setShapeList(_shapes);
    return 0;
}
