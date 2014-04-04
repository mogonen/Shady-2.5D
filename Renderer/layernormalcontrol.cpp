#include "layernormalcontrol.h"
#include <qdebug>
#include <qgl.h>

void LayerNormalControl::onDrag(const Point &t, int button)
{
    if(_EndPoint.norm()>_Radius)
        _EndPoint.set(_EndPoint - t);
}

QVector3D  LayerNormalControl::Normal3D()
{
    return QVector3D(P().x,P().y,sqrt(_Radius*_Radius-P().x*P().x-P().y*P().y))/_Radius;
}

void  LayerNormalControl::SetNormal3D(QVector3D normal3D)
{

    pP()->set(normal3D.x()*_Radius,normal3D.y()*_Radius);
}

