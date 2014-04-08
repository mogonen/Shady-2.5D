#ifndef LAYERNORMALCONTROL_H
#define LAYERNORMALCONTROL_H

#include "shape.h"
#include "controlpoint.h"

class LayerNormalControl:public ControlPoint
{

protected:
    Point       _EndPoint;
    Shape_p     _pShape;

    void onDrag(const Point &t, int button);

public:

    LayerNormalControl(Selectable_p shape = NULL):ControlPoint(&_EndPoint, shape)
    {
        _Radius = 0.2;
        _pShape = (Shape_p)shape;
        makeDraggable();
    }

    void render(int mode = 0);

    QVector3D  Normal3D();
    //void SetNormal3D(QVector3D normal3D);

private:
    float      _Radius;
};



#endif // LAYERNORMALCONTROL_H
