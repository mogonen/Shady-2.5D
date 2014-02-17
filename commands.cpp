#include "commands.h"

Drag::DragTool     Drag::TOOL = Drag::NONE;
Drag::Continuity   Drag::CONT = Drag::C1;

Drag::Drag(Draggable_p pObj, const Vec2& t){
    _pObj = pObj;
    _t = t;
}

Command_p Drag::exec(){
    return 0;
}

Command_p Drag::unexec(){
    _pObj->drag(-_t);
    return 0;
}
