#include "shape.h"

//ShapeVertex///////////////////////////////////////////////////////////////////////////
int ShapeVertex::_COUNT = 0;
SVMap ShapeVertex::_svmap;

ShapeVertex::ShapeVertex(Shape_p pS){
    _pShape = pS;
    N.set(0,0,1);
    flag = 0x00;
    _id = _COUNT;
    _COUNT++;
    _parent = _pair = 0;
    pRef = 0;
    _svmap[_id] = this;
    isPositionControl = isNormalControl = true;
}

void ShapeVertex::adopt(ShapeVertex_p sv){
    if (sv->_parent)
        sv->_parent->_childs.remove(sv);
    sv->_parent = this;
    _childs.push_back(sv);
}

void ShapeVertex::setPair(ShapeVertex_p sv,bool isSetTangent,  bool isSetNormal){
    if (!sv || sv->_parent != _parent)
        return;

    if (sv->_pair)
        sv->_pair->_pair = 0; //unpair
    sv->_pair = this;
    if (_pair)
        _pair->_pair = 0;//unpair
    _pair = sv;

    Vec2 tan = (P - _pair->P);
    if (isSetNormal){
        _parent->N = Vec3(0,0,1)%Vec3(tan).normalize();
    }
    if (isSetTangent){
        P.set(_parent->P + tan*0.5);
        _pair->P.set(_parent->P - tan*0.5);
    }
}

void ShapeVertex::unpair(){
    if (_pair)
        _pair->_pair =0;
    _pair =0;
}

ShapeVertex_p ShapeVertex::get(int id){
    SVMap::const_iterator it = _svmap.find(id);
    if (it == _svmap.end())
        return 0;
    return it->second;
}

ShapeVertex::~ShapeVertex(){
    _svmap.erase(_id);
    if (_childs.size()){
        FOR_ALL_ITEMS(SVList, _childs)
           (*it)->_parent = 0;
    }
    if (_parent)
        _parent->_childs.remove(this);
    if (_pair)
        _pair->_pair =0;
}

void ShapeVertex::drag(const Vec2 &t, bool isNormal){

    _pShape->outdate(this);
    if (isNormal){

        Vec2 v = (Vec2(N.x*NORMAL_RAD, N.y*NORMAL_RAD) + t);
        double l = v.norm();
        if ( l > NORMAL_RAD ){
            v = v.normalize()*NORMAL_RAD;
            l = NORMAL_RAD;
        }
        double h = sqrt(NORMAL_RAD*NORMAL_RAD - l*l);
        if (h < 0)
            h = 0;
        N.set(Vec3(v.x, v.y, h).normalize());
        return;
    }

    P = P + t;

    if (_pair){

        Vec2 tan  = (_parent->P - P).normalize();
       _pair->P.set(_parent->P + tan*(_pair->P - _parent->P).norm());

       //now rotate the normal
       Vec2 n2d(_parent->N);
       Vec2 tan0 = (_parent->P - P + t).normalize();
       Vec2 y_ax0 = Vec3(0,0,1)%Vec3(tan0.x, tan0.y, 0);
       Vec2 y_ax1 = Vec3(0,0,1)%Vec3(tan.x, tan.y, 0);

       Vec2 n2d_1 = tan*(tan0*n2d) + y_ax1*(y_ax0*n2d);
       Vec3 n3d(n2d_1.x, n2d_1.y, _parent->N.z);
       _parent->N = n3d.normalize();
       _pShape->outdate(_pair);
    }

    if (hasChilds()){
        FOR_ALL_CONST_ITEMS(SVList, _childs){
            (*it)->P = (*it)->P + t;
        }
    }
}

void   ShapeVertex::setTangent(const Vec2& tan, bool isnormal, bool ispair){
    if (!_parent)
        return;

    P.set(_parent->P + tan);

    if (isnormal)
        _parent->N = Vec3(0,0,1)%Vec3(tan).normalize();

    if (ispair&&_pair)
        _pair->P.set(_parent->P - tan);
}

Vec2 ShapeVertex::getTangent(){
    if (_parent)
        return (P - _parent->P);

    return Vec2(0,0);
}


//Shape////////////////////////////////////////////////////////////////////////////////////

Shape::Shape():Draggable(Renderable::SHAPE, &_t0){
    _flags = 0;
    _tM.identity();
    diffuse.setRed(255);
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
    ShapeVertex_p sv = addVertex();
    sv->P = p;
    sv->_parent = parent;
    if (parent){
        parent->adopt(sv);
    }
    sv->isPositionControl  = isPositionControl;
    sv->isNormalControl    = isNormalControl;
    return sv;
}

void Shape::removeVertex(ShapeVertex_p sv){
    _vertices.remove(sv);
    delete sv;
}

void Shape::removeVertex(Point_p pP){
    SVList::iterator it = _vertices.begin();
    for(;it!=_vertices.end() && (*it)->pP() != pP; it++);
    if (it!=_vertices.end()){
        ShapeVertex_p sv =*it;
        _vertices.erase(it);
        delete sv;
    }
}

Point Shape::gT(){
    if (!parent())
        return _t0;
    return _t0 + ((Shape_p)parent())->gT();
}

void Shape::sendClick(const Point& p, Click_e click){
    Point p0(p);
    p0 = p0 - _t0;
    onClick(p0, click);
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

void Shape::applyT(const Matrix3x3& M){
    FOR_ALL_ITEMS(SVList, _vertices){
        ShapeVertex_p sv = (*it);
        Vec3 p = M*Vec3(sv->P.x, sv->P.y, 1);
        Vec3 n = M*Vec3(sv->N.x, sv->N.y, 0);
        sv->P.set(p.x/p.z, p.y/p.z);
        sv->N.set(Vec3(n.x, n.y,sv->N.z).normalize());
    }
    onApplyT(M);
    Renderable::update();
}

void Shape::frezeT(){
    applyT(_tM);
    _tM.identity();
    //centerPivot();
}

void Shape::getBBox(BBox &bbox) const{
    bbox.P[0].set(99999, 99999);
    bbox.P[1].set(-99999, -99999);

    FOR_ALL_CONST_ITEMS(SVList, _vertices){
        ShapeVertex_p sv = (*it);
        //if (sv->parent()) continue;
        if (sv->P.x < bbox.P[0].x)
            bbox.P[0].x = sv->P.x;
        if (sv->P.y < bbox.P[0].y)
            bbox.P[0].y = sv->P.y;

        if (sv->P.x > bbox.P[1].x)
            bbox.P[1].x = sv->P.x;
        if (sv->P.y > bbox.P[1].y)
            bbox.P[1].y = sv->P.y;
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
        sv->P = sv->P - piv;
    }
    _t0 = _t0 + piv;
    Renderable::update();
}
