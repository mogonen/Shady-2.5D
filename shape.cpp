#include "shape.h"
#include "commands.h"
#include "Renderer/layernormalcontrol.h"

//ShapeVec
ShapeVec& ShapeVec::operator= (const ShapeVec &v)
{
    // do the copy
    //_P = v._P;
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        value[i] = v.value[i];

    return *this;
}

ShapeVec operator+(const ShapeVec& v1, const ShapeVec& v2){
    ShapeVec v;
    //v._P = v1._P + v2._P;
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        v.value[i] = v1.value[i] + v2.value[i];
    return v;
}

ShapeVec operator-(const ShapeVec& v1, const ShapeVec& v2){
    ShapeVec v;
    //v._P = v1._P - v2._P;
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        v.value[i] = v1.value[i] - v2.value[i];
    return v;
}

ShapeVec operator*(const ShapeVec& v, double s){
    ShapeVec r;
    //r._P = v._P*s;
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        r.value[i] = v.value[i]*s;
    return r;
}



//ShapeVertex///////////////////////////////////////////////////////////////////////////

ShapeVertex::ShapeVertex(Shape_p pS, bool isP, bool isN):ControlPoint(&_P), ShapeBase(isN){
    _pShape = pS;

#ifdef RENDERING_MODE
    //init channels
    value[NORMAL_CHANNEL]    = _pShape->value[NORMAL_CHANNEL];
    value[BRIGHT_CHANNEL]    = _pShape->value[BRIGHT_CHANNEL];
    value[DARK_CHANNEL]      = _pShape->value[DARK_CHANNEL];
    value[DEPTH_CHANNEL]     = _pShape->value[DEPTH_CHANNEL];
    isNormalControl = isN;
    if (isN){
        adopt(pControlN());
    }
    //value[ALPHA_CHANNEL].set(1.0,1.0,1.0);
#endif

    flag = 0x00;
     _pair = 0;
    isPositionControl = isP;
    _isDeleted = false;
}

void ShapeVertex::setPair(ShapeVertex_p sv,bool isSetTangent,  bool isSetNormal){
    if (!sv || sv->parent() != parent())
        return;

    if (_pair && !_pair->isDeleted() && _pair->_pair == this)
        _pair->_pair = 0;//unpair if alive

    _pair = sv;

    if (sv->_pair && !sv->_pair->isDeleted())
        sv->_pair->_pair = 0; //unpair if alive*/

    sv->_pair = this;

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

Point   ShapeVertex::gP(){
    return _pShape? (P()+ _pShape->gT()) : P();
}

void    ShapeVertex::setGlobalP(const Point& p){
    pP()->set(p - _pShape->gT());
}

/*
*/

SVCache::SVCache(ShapeVertex_p sv){
    _sv = sv;
    _pair = sv->pair();
    _parent = (ShapeVertex_p)sv->parent();
    _childs = sv->getChilds();
    _P = _sv->P();
    _N = _sv->N();
}

void SVCache::restore(){

    _sv->value[NORMAL_CHANNEL] = _N;
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


ShapeBase::ShapeBase(bool isN){
#ifdef RENDERING_MODE
    value[NORMAL_CHANNEL].set(0.0, 0.0, 1.0);
    value[BRIGHT_CHANNEL].set(1.0, 1.0, 1.0);
    value[DARK_CHANNEL].set(0.0, 0.0, 0.0);
    value[DEPTH_CHANNEL].set(0.5, 0.5, 0.5);

    if (isN){
        _pControlN = new ControlNormal(this);
    }else
        _pControlN = 0;


#endif
}

void ShapeBase::dragNormal(const Vec2 &t)
{
#ifdef RENDERING_MODE
    double r = NORMAL_RAD*2;
    Vec2 v = (Vec2(value[NORMAL_CHANNEL].x*r, value[NORMAL_CHANNEL].y*r) + t);
    double l = v.norm();
    if ( l > r ){
        v = v.normalize()*r;
        l = r;
    }

    double h = sqrt(r*r - l*l);
    if (h < 0)
        h = 0;

    value[NORMAL_CHANNEL].set(Vec3(v.x, v.y, h).normalize());
    outdate();

#endif
}


//Shape////////////////////////////////////////////////////////////////////////////////////
Shape::Shape():Draggable(false, &_P), ShapeBase(true)
{
    _flags = 0;
    _tM.identity();

#ifndef MODELING_MODE

    m_alpha_th      = 0.1;
    m_stretch       = 0;
    m_assignedDepth = 0.5;
    m_shadowCreator = true;

    pControlN()->_color[0] = 1.0;
    pControlN()->_color[1] = 1.0;
    pControlN()->_color[2] = 0.0;

#endif

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

Point Shape::gT()
{
    if (!parent())
        return _P;
    return _P + ((Shape_p)parent())->gT();
}

void Shape::sendClick(const Click& click){
    Click click2 = click;
    click2.P = click2.P - _P;
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
    _P.set();
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
    _P = _P + piv;
    Renderable::update();
}

Point ControlNormal::P() const
{
   Point p;
   if (parent()){
        p = parent()->P();
        return p + Point(_pShapeBase->N()*NORMAL_RAD);
   }
   return Point(_pShapeBase->N()*NORMAL_RAD*2);
}

void ControlNormal::onDrag(const Point &t, int button){
    _pShapeBase->dragNormal(t);
}

void SBCache::set(ShapeBase_p pSB){
    _pSB = pSB;
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        _value[i] = pSB->value[i];
    _p = pSB->_P;
}

void SBCache::restore(){
    for(int i=0; i<ACTIVE_CHANNELS; i++)
        _pSB->value[i] = _value[i];
    _pSB->_P = _p;
    _pSB->outdate();
}

#ifndef MODELING_MODE
void Shape::assignDepthValues()
{
    BBox bbox;
    getBBox(bbox);

    FOR_ALL_ITEMS(SVList, _vertices)
    {
        //  glColor4f(c, blf, ((float)_shaderParam.m_layerLabel + 1.01)/255.0, 1.0);
        ShapeVertex* pSV = *it;
        Point p = pSV->_P;

        Vec2 v = (p - bbox.P[0]);
        double x = v.x / bbox.diag().x,  y = v.y / bbox.diag().y;
        double zb = cornerz[0] * (1-x) + cornerz[1]*x;
        double zt = cornerz[2] * (1-x) + cornerz[3]*x;

        double z = zb*(1-y) + zt*y;

        //double z = ;
        (*it)->value[DEPTH_CHANNEL] = Vec3(m_assignedDepth, z, ((float)_shaderParam.m_layerLabel + 1.01)/255.0);
    }
}

Matrix3x3 Shape::getPreviewM()
{

    Vec3 n = value[NORMAL_CHANNEL];
    //qDebug()<<layerNormal;
    double d0;
    double d1;

    BBox bbox;
    getBBox(bbox);

    if(n.z>0.1)
    {
        d0 = -(n.x*bbox.diag().x + n.y*bbox.diag().y) / n.z;
        d1 = -(n.x*bbox.diag().x - n.y*bbox.diag().y) / n.z;
    }
    else
    {
        d0 = 1;
        d1 = 1;
    }

    double c = this->m_assignedDepth;

    cornerz[0] = (c - d0);
    cornerz[1] = (c + d1);

    cornerz[2] = (c - d1);
    cornerz[3] = (c + d0);

    //1.1 is for numerical issue
    //float blf = (c - d0);
    //float brf = (c + d1);

    //float tlf = (c - d1);
    //float trf = (c + d0);

    //trf = CapValue(trf,0,1);
    //brf = CapValue(brf,0,1);
    //tlf = CapValue(tlf,0,1);
    //blf = CapValue(blf,0,1);

    double yy = (d0 - d1);
    double xx = (- d1 - d0);

    //double yy = (CLAMP(c - d1, 0, 1) + CLAMP(c + d0, 0, 1))/2.0 - c; //(d0 - d1);
    //double xx = (CLAMP(c - d0, 0, 1) + CLAMP(c - d1, 0, 1))/2.0 - c;//( - d1 - d0);

    double y = yy / bbox.diag().y;
    double x = xx / bbox.diag().x;

    Matrix3x3 m;
    m.set(1,0,0, 0,1,0, -x, y, 1);
    //m.set(1,0,x, 0,1,y, 0, 0, 1);
    return m;
}


void Shape::getPrevParam(double * val){

    val[0] = m_alpha_th;
    val[1] = m_stretch;
    val[2] = m_assignedDepth;
    val[3] = m_shadowCreator;
}

void Shape::setPrevParam(double * val){
    m_alpha_th      = val[0];
    m_stretch       = val[1];
    m_assignedDepth = val[2];
    m_shadowCreator = val[3];
}

#endif
