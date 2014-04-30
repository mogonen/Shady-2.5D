#include "cmesh.h"

using namespace dlfl;

Corner::Corner(Vertex_p v, Face_p f){
    if (v)
        v->set(this);

    if (v && !f){
        f = v->mesh()->addFace(1, true);
        f->set(this);
    }

    _f = f;
    _e = 0;
    _i = -1;
    _next = _prev = 0;
}

Corner::~Corner(){
   /* if (_v && _v->C() == this)
        _v->set(vNext());

    if (_e && _e->C0() == this)
        _e->set(0,0);
    else if (_e && _e->C1() == this)
        _e->set(0,1);

    /*if (_next && _next->_prev == this)
        _prev->setNext(_next);*/
}

void Corner::setNext(Corner_p c){
    _next = c;
    if (c)
        c->_prev = this;
}

//Point Corner::P(){ return *_v->pP;}

int    Corner::I(){return _i;}

Edge_p	Corner::E() const {return _e;}
Face_p	Corner::F() const {return _f;}
Vertex_p Corner::V() const {return _v;}

Corner_p Corner::next(){
    return _next?_next:( (_f&&_i>=0)?_f->C(_i+1) : 0);
}

Corner_p Corner::prev(){
    return _prev?_prev:( (_f&&_i>=0)?_f->C(_i-1) : 0);
}

Corner_p Corner::vNext(){
    return _e? _e->other(this)->next():0;
}

Corner_p Corner::vPrev(){
    return _prev ? prev()->_e->other(prev()) : 0;
}

Corner_p Corner::other(){
    return _e? _e->other(this) : 0;
}

Corner_p Corner::go(int i){
    Corner_p c = this;
    while(i!=0){
        if (i>0){
            c = c->next();
            i--;
        }else{
            c = c->prev();
            i++;
        }
    }
    return c;
}

Corner_p Corner::vNextBorder(){
    Corner_p vnext = this->vNext();
    while(vnext->F() && vnext!=this)
        vnext = vnext->vNext();
    return (vnext==this)?0:vnext;
}

Corner_p Corner::vPrevBorder(){
    Corner_p vprev = this->vPrev();
    while(vprev->F() && vprev!=this)
        vprev = vprev->vPrev();
    return (vprev==this)?0:vprev;
}


bool Corner::isC1(){return _e->isC1(this);}
bool Corner::isBorder(){return !_f || _f->isBorder();}

void Vertex::set(Corner_p c){
    _c = c;
    if (_c)
        _c->_v = this;
}

Corner_p Vertex::to(Vertex_p v){
    if (!_c || !_c->next())
        return 0;
    Corner_p c = _c;
    while(c->next()->V()!= v){
        c = c->vNext();
        if (c==_c)
            return 0;
    }
    return c;
}

Corner_p Vertex::from(Vertex_p v){
    if (!_c || !_c->prev())
        return 0;
    Corner_p c = _c;
    while(c->prev()->V()!= v){
        c = c->vNext();
        if (c==_c)
            return 0;
    }
    return c;
}

Corner_p Vertex::find(Face_p f){
    if (!_c)
        return 0;
    Corner_p c = _c;
    while((c->F()!= f) && !(c->F()->isBorder()&&f->isBorder()) ){
        c = c->vNext();
        if (c==_c)
            return 0;
    }
    return c;
}

int Vertex::edgeId(const Vertex_p v) const {return id() + v->id()*mesh()->size();}
int Vertex::edgeUId(const Vertex_p v) const {return (id() > v->id())?(v->id() + id()*mesh()->size()):(id() + v->id()*mesh()->size());}

Face::Face(int s){
    _isBorder = false;
    _corns = 0;
    pData  = 0;
    init(s);
}

void Face::init(int s){
    _size = s;
    if (_corns)
        delete [] _corns;
    _corns = new Corner_p[_size];
    for(int i=0; i<_size; i++){
        Corner_p cp = new Corner();
        set(cp, i);
    }
    update();
}

Corner_p  Face::C(int i) const {return _corns[(i+_size)%_size]; }
Vertex*   Face::V(int i) const {return _corns[(i+_size)%_size]->V(); }

void Face::set(Corner_p c, int i){

    if (i<0){
        c->_f = this;
        return;
    }

    int ii = (i+_size)%_size;
    _corns[ii] = c;
    c->_f = this;
    c->_i = ii;
}

void Face::set(Vertex* v, int i){
    v->set(C((i+_size)%_size));
}

void Face::update(bool links, int offset){

    Corner_p ci = _corns[0];
    if (links){
        _size = 1;
        for(Corner_p c = _corns[0]->next(); c != _corns[0]; c = c->next())
            _size++;
        delete _corns;
        _corns = new Corner_p[_size];
    }

    for(int i=0; i<_size; i++){
        if (links){
            _corns[i%_size] = ci;
            ci = ci->next();
        }else
            C(i-1)->setNext(_corns[i]);
        set(_corns[i], i);
    }

    if (offset)
        reoffset(offset);
}

void Face::reoffset(int off){
    if (off==0)
        return;

    if (off < 0)
        off = (_size*10 + off) % _size;

    Corner_p c0 = C(0);
    for(Corner_p c = c0->next(); c!=c0; c = c->next())
        set(c, c->I() + off);
    set(c0, c0->I() + off);
}

void Face::remove(bool lazydel){
    EdgeList todel;
    for(int i=0; i<_size; i++)
        if (C(i)->E()->isBorder())
            todel.push_back(C(i)->E());

    for(EdgeList::iterator it = todel.begin(); it!=todel.end(); it++)
        mesh()->remove(*it, lazydel);

    _isBorder = true;
}

int  Face::size()       const {return _size;}
bool Face::isBorder()   const {return _isBorder;}
bool Edge::isBorder(){return !_c0 || _c0->isBorder() || !_c1 || _c1->isBorder();}

void Edge::set(Corner_p c, int i){

    if (i == -1){
         if (!_c0)
            _c0 = c;
        else
            _c1 = c;
    }else if (i == 0){ //now  override
        if(_c0)
            _c0->_e = 0;
        _c0 = c;
    }else if (i==1){
        if(_c1)
            _c1->_e = 0;
        _c1 = c;
    }

    if (c)
        c->_e = this;
}

Corner_p Edge::C(int i)const {
    return (i==-1)? ((!_c1->isBorder() && _c0->isBorder())?_c1 : _c0) : ((i==0)?_c0:_c1);
}

Edge::Edge(){
    _c0 = _c1 = 0;
    pData = 0;
}
