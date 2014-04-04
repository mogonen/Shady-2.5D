#include "shape.h"
#include "commands.h"
#include "Renderer/layernormalcontrol.h"

//ShapeVertex///////////////////////////////////////////////////////////////////////////

ShapeVertex::ShapeVertex(Shape_p pS, bool isP, bool isN):ControlPoint(&_P){
    _pShape = pS;

    //init channels
    data[NORMAL_CHANNEL].set(0,0,1);
    data[BRIGHT_CHANNEL].set(1,1,1);
    data[DARK_CHANNEL].set(0,0,0);
    data[DEPTH_CHANNEL].set(0.5, 0.5, 0.5);
    //data[ALPHA_CHANNEL].set(1.0,1.0,1.0);

    flag = 0x00;
     _pair = 0;
    isPositionControl = isP;
    isNormalControl = isN;
    if (isN){
        _pControlN = new ControlNormal(this);
        adopt(_pControlN);
    }else
        _pControlN = 0;

    _isDeleted = false;
}

void ShapeVertex::setPair(ShapeVertex_p sv,bool isSetTangent,  bool isSetNormal){
    if (!sv || sv->parent() != parent())
        return;

    if (sv->_pair)
        sv->_pair->_pair = 0; //unpair
    sv->_pair = this;
    if (_pair)
        _pair->_pair = 0;//unpair
    _pair = sv;

    Vec2 tan = (_P - _pair->_P);
    if (isSetNormal){
        ((ShapeVertex*)parent())->pN()->set(Vec3(0,0,1)%Vec3(tan).normalize());
    }
    if (isSetTangent){
        _P.set(parent()->P() + tan*0.5);
        _pair->_P.set(parent()->P() - tan*0.5);
    }
}

void ShapeVertex::unpair(){
    if (_pair)
        _pair->_pair =0;
    _pair = 0;
}

ShapeVertex::~ShapeVertex(){

    if (_pair)
        _pair->_pair =0;
}

void ShapeVertex::outdate()
{
    _pShape->outdate(this);
}

void ShapeVertex::onDrag(const Vec2 &t, int button){

    //if (button==)

    _pShape->outdate(this);
    _pShape->drag(this, t);

    if (_pair && !_pair->_isDeleted){

        Vec2 tan  = (parent()->P() - _P);
        /*if (isC2){
            _pair->_P.set(parent()->P() + tan);
            tan = tan.normalize();
        }else{*/
            tan = tan.normalize();
            _pair->_P.set(parent()->P() + tan*(_pair->_P - parent()->P()).norm());
        //}

       //now rotate the normal
       Vec3 parentN = ((ShapeVertex_p)parent())->N();
       Vec2 n2d(parentN);
       Vec2 tan0 = (parent()->P() - _P + t).normalize();
       Vec2 y_ax0 = Vec3(0,0,1)%Vec3(tan0.x, tan0.y, 0);
       Vec2 y_ax1 = Vec3(0,0,1)%Vec3(tan.x, tan.y, 0);

       Vec2 n2d_1 = tan*(tan0*n2d) + y_ax1*(y_ax0*n2d);
       Vec3 n3d(n2d_1.x, n2d_1.y, parentN.z);
       ((ShapeVertex_p)parent())->pN()->set(n3d.normalize());
       _pShape->outdate(_pair);
    }
    Draggable::onDrag(t, button);
}

void ShapeVertex::dragNormal(const Vec2 &t){
    _pShape->outdate(this);

    Vec2 v = (Vec2(data[NORMAL_CHANNEL].x*NORMAL_RAD, data[NORMAL_CHANNEL].y*NORMAL_RAD) + t);
    double l = v.norm();
    if ( l > NORMAL_RAD ){
        v = v.normalize()*NORMAL_RAD;
        l = NORMAL_RAD;
    }
    double h = sqrt(NORMAL_RAD*NORMAL_RAD - l*l);
    if (h < 0)
        h = 0;
    data[NORMAL_CHANNEL].set(Vec3(v.x, v.y, h).normalize());
}

void   ShapeVertex::setTangent(const Vec2& tan, bool isnormal, bool ispair){
    if (!parent())
        return;

    _P.set(parent()->P() + tan);

    if (isnormal){
        ((ShapeVertex*)parent())->pN()->set((Vec3(0,0,1)%Vec3(tan)).normalize());
    }
    if (ispair&&_pair)
        _pair->_P.set(parent()->P() - tan);
}

Vec2 ShapeVertex::getTangent(){
    if (parent())
        return (_P - parent()->P());

    return Vec2(0,0);
}

SVCache::SVCache(ShapeVertex_p sv){
    _sv = sv;
    _pair = sv->pair();
    _parent = (ShapeVertex_p)sv->parent();
    _childs = sv->getChilds();
    _P = _sv->P();
    _N = _sv->N();
}

void SVCache::restore(){

    _sv->data[NORMAL_CHANNEL] = _N;
    _sv->_P = _P;
    _sv->unparent();
    if (_parent){
        _parent->adopt(_sv);
    }

    if (_pair)
        _sv->setPair(_pair);

    DraggableList childs = _sv->getChilds();
    FOR_ALL_ITEMS(DraggableList, childs){
        (*it)->unparent();
    }

    if (!_childs.empty()){
        FOR_ALL_ITEMS(DraggableList, _childs){
            ShapeVertex_p child = (ShapeVertex_p)(*it);
            child->unparent();
            _sv->adopt(child);
        }
    }

    //_sv->isNormalControl = _sv->isParent();
}

//Shape////////////////////////////////////////////////////////////////////////////////////
Shape::Shape():Draggable(false, &_t0)
{
    _flags = 0;
    _tM.identity();
    diffuse.setRed(255);

    _layerNormal    = QVector3D(0.0,0.0,1.0);
    _NormalControl  = new LayerNormalControl(this);
}

Shape::~Shape(){
    FOR_ALL_ITEMS(SVList, _vertices)
        delete (*it);
}

ShapeVertex_p Shape::addVertex()
{
    ShapeVertex_p sv = new ShapeVertex(this);
    _vertices.push_back(sv);
    return sv;
}

ShapeVertex_p Shape::addVertex(const Point& p, ShapeVertex_p parent, bool isPositionControl, bool isNormalControl){
    ShapeVertex_p sv = new ShapeVertex(this, isPositionControl, isNormalControl);
    _vertices.push_back(sv);
    sv->_P = p;
    if (parent){
        parent->adopt(sv);
    }
    return sv;
}

void Shape::removeVertex(ShapeVertex_p sv){
    sv->_isDeleted = true;
    //_vertices.remove(sv);
    //delete sv;
}

Point Shape::gT(){
    if (!parent())
        return _t0;
    return _t0 + ((Shape_p)parent())->gT();
}

void Shape::sendClick(const Click& click){
    Click click2 = click;
    click2.P = click2.P - _t0;
    onClick(click2);
}

void Shape::translate(const Vec2& t){
    _tM = _tM + Matrix3x3(0,0,t.x, 0,0, t.y,0,0,0);
}

void Shape::rotate(double ang){
    Matrix3x3 rM(cos(ang), -sin(ang), 0, sin(ang), cos(ang), 0, 0,0,1);
    _tM = rM*_tM;
}

void Shape::scale(const Vec2& s){
    Matrix3x3 sM(s.x,0,0, s.y,0,0, 0,0,1);
    _tM = sM*_tM;
}

void Shape::resetT(){
    _tM.identity();
    _t0.set();
}

void Shape::drag(ShapeVertex_p sv, const Vec2 & t){
    onDrag(sv, t);
}

void Shape::applyT(const Matrix3x3& M){
    FOR_ALL_ITEMS(SVList, _vertices){
        ShapeVertex_p sv = (*it);
        Vec3 p = M*Vec3(sv->_P.x, sv->_P.y, 1);
        //Vec3 n = M*Vec3(sv->_N.x, sv->_N.y, 0);
        sv->_P.set(p.x/p.z, p.y/p.z);
        //sv->_N.set(Vec3(n.x, n.y,sv->_N.z).normalize());
    }
    onApplyT(M);
    Renderable::update();
}

void Shape::frezeT(){
    applyT(_tM);
    _tM.identity();
    diffuse.setRed(250);
    //centerPivot();
}

void Shape::getBBox(BBox &bbox) const
{
    bbox.P[0].set(99999, 99999);
    bbox.P[1].set(-99999, -99999);
    FOR_ALL_CONST_ITEMS(SVList, _vertices){
        ShapeVertex_p sv = (*it);
        //if (sv->_Parent()) continue;
        if (sv->_P.x < bbox.P[0].x)
            bbox.P[0].x = sv->_P.x;
        if (sv->_P.y < bbox.P[0].y)
            bbox.P[0].y = sv->_P.y;

        if (sv->_P.x > bbox.P[1].x)
            bbox.P[1].x = sv->_P.x;
        if (sv->_P.y > bbox.P[1].y)
            bbox.P[1].y = sv->_P.y;
    }
}

void Shape::centerPivot(){

   if(_vertices.empty())
        return;

    BBox bbox;
    getBBox(bbox);
    Point piv = bbox.pivot();
    FOR_ALL_ITEMS(SVList, _vertices){
        ShapeVertex_p sv = (*it);
        sv->_P = sv->_P - piv;
    }
    _t0 = _t0 + piv;
    Renderable::update();
}
