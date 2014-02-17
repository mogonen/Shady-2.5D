#ifndef COMMANDS_H
#define COMMANDS_H

#include "shape.h"

class Drag:public Command{

    Vec2 _t;
    Draggable_p     _pObj;

public:

    Drag(Draggable_p pObj, const Vec2& t);

    enum DragTool {NONE, AUTO_BIND, MANUAL_BIND, BREAK};
    enum Continuity {C0, C1, C2};

    Command_p exec();
    Command_p unexec();
    CommandType  type() const {return DRAG;}

    static DragTool     TOOL;
    static Continuity   CONT;
};



#endif // COMMANDS_H
