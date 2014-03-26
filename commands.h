#ifndef COMMANDS_H
#define COMMANDS_H

#include "shape.h"

class Drag:public Command{

    Vec2 _t;
    Draggable_p     _pObj;
    SelectionSet    _selection;

public:

    Drag(Draggable_p pObj, const Vec2& t);
    Drag(SelectionSet set, const Vec2& t);

    enum DragTool {NONE, AUTO_BIND, MANUAL_BIND, BREAK};
    enum Continuity {C0, C1, G1};

    Command_p       exec();
    Command_p       unexec();
    CommandType     type() const {return DRAG;}

    static DragTool     TOOL;
    static Continuity   CONT;
};

class ShapeOrder:public Command{

    ShapeList   _shapes;
    Shape_p     _pShape;

public:

    enum Operation {MOVE_UP, MOVE_DOWN, SEND_BACK, SEND_FRONT, INSERT_SHAPE, DELETE_SHAPE};

    Command_p exec();
    Command_p unexec();

    CommandType  type() const {return SHAPE_ORDER;}


    ShapeOrder(Operation op);
    void setShape(Shape_p pShape){_pShape = pShape;}

private:

    Operation _operation;

};


#endif // COMMANDS_H
