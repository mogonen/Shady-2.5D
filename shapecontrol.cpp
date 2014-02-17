#include "shapecontrol.h"
#include "commands.h"

//ShapeControl///////////////////////////////////////////////////////////////////////////////////

ShapeControl::ShapeControl():Draggable(true, &_t0){
    isNormalControl = false;
    _theSelected = 0;
    _theHandler = new TransformHandler();
}

void ShapeControl::startSelect(unsigned int svname){
}

void ShapeControl::onDrag(const Vec2& t, int){

}

