#include "commands.h"

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
