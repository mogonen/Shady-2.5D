#include "CMesh.h"

using namespace dlfl;

Mesh::Mesh(){
    resetCB();
}

void Mesh::resetCB(){
    _insertEdgeCB = 0;
    _splitEdgeCB  = 0;
    _addFaceCB	  = 0;
    _removeFaceCB = 0;
    _removeEdgeCB = 0;
    _caller = 0;
}

Vertex* Mesh::addVertex(VertexData *pData){
    Vertex* v = new Vertex();
	_verts.push_back(v);
    v->pData = pData;
	v->_mesh = this;
	return v;
}

Face_p Mesh::addFace(int s){
    Face_p f = new Face(s);
	f->_mesh = this;
	_faces.push_back(f);
	if (_addFaceCB)
		_addFaceCB(f);
	return f;
}

Face_p Mesh::addQuad(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3){

    Face_p f = addFace(4);
	f->set(v0, 0);
	f->set(v1, 1);
	f->set(v2, 2);
	f->set(v3, 3);
	f->update();
	return f;
}

Edge_p Mesh::addEdge(Corner_p c0, Corner_p c1){
    Edge_p e = new Edge();
	e->_mesh = this;
	_edges.push_back(e);

	if (c0)
		e->set(c0);

	if (c1)
		e->set(c1);

	if (_insertEdgeCB)
		_insertEdgeCB(e);

	return e;
}

void Mesh::remove(Face_p f, bool lazydel){
	if (_removeFaceCB)
		_removeFaceCB(f);
	f->remove();
	if (lazydel){
		f->markDeleted();
	}else{
		_faces.remove(f);
		delete f;
	}
}
void Mesh::remove(Edge_p e, bool lazydel){
	if (_removeEdgeCB)
		_removeEdgeCB(e);

	e->remove();
	if (lazydel){
		e->markDeleted();
	}else{
		_edges.remove(e);
		delete e;
	}
}

Corner_p Mesh::splitEdge(Edge_p pE, Vertex_p pV, Face_p f){

    Vertex* v = pV? pV : addVertex();

    Corner_p csnew = 0;
    Corner_p c0new = 0;
    if (pE->C0()){
        c0new = new Corner();
        v->set(c0new);
        pE->C0()->F()->set(c0new, -1);

        Corner_p c0n = pE->C0()->next();
        pE->C0()->setNext(c0new);
        c0new->setNext(c0n);
    }

    Corner_p c1new=0;
    Corner_p c1old = pE->C1();
    if (pE->C1()){
        c1new = new Corner();
        v->set(c1new);
        pE->C1()->F()->set(c1new, -1);

        Corner_p c1n = pE->C1()->next();
        pE->C1()->setNext(c1new);
        c1new->setNext(c1n);
        pE->set(c1new, 1);
    }

    Edge_p enew = addEdge(c0new, c1old);
    Corner_p cc = f?((f==c0new->F())?c0new:c1new):c0new;

    if (_splitEdgeCB)
        _splitEdgeCB(cc);

    return cc;
}

void Mesh::updateF(){
    for(std::list<Face_p>::iterator itf = _faces.begin(); itf!=_faces.end(); itf++)
		(*itf)->update();
}

void Mesh::enamurateVerts(){
	int id = 0;
    for(std::list<Vertex*>::iterator it = _verts.begin(); it!=_verts.end(); it++)
		(*it)->_id = id++;
}

void Mesh::buildEdges(){
	_edges.clear();
	enamurateVerts();
	int es = size()*size();
    Edge_p* ematrix = new Edge_p[es];
	for(int i=0; i < es; i++)
		ematrix[i] = 0x00;
    for(std::list<Face_p>::const_iterator it = _faces.begin(); it != _faces.end(); it++){
        Face_p f = *it;
		for(int i = 0; i < f->size(); i++ ){
			int eid = f->V(i)->edgeUId(f->V(i+1));
			if (ematrix[eid]){
				ematrix[eid]->set(f->C(i),1);
			}else{
                Edge_p e = addEdge(f->C(i), 0);
				ematrix[eid] = e;
			}
		}
	}
}

Corner::Corner(){
        _v = 0; // Vertex::nullV;
		_e = 0;
		_f = 0;
		_i = 0;
	}

void Corner::setNext(Corner_p c){
	_next = c;
	if (c)
		c->_prev = this;
}

//Point Corner::P(){ return *_v->pP;}

Corner_p Corner::next(){
	return _next?_next:_f->C(_i+1);
}

Corner_p Corner::prev(){
	return _prev?_prev:_f->C(_i-1); 
}

Corner_p Corner::vNext(){
    Corner_p _c1 = _e->other(this);
	return (_c1)?_c1->next():0;	
}

Corner_p Corner::vPrev(){
	return prev()->_e->other(prev());
}

Corner_p Corner::other(){
	return (this==_e->C0())?_e->C1():((this == _e->C1())?_e->C0():0);
}

bool Corner::isC0(){return this == _e->C0();}
bool Corner::isBorder(){return _e->isBorder() ||  _prev->_e->isBorder();}


void Vertex::set(Corner_p c){
    _c = c;
    _c->_v = this;
}

Face::Face(int s){

    pData = 0;
	_size = s;
    _corns = new Corner_p[_size];
	for(int i=0; i<_size; i++){
        Corner_p cp = new Corner();
        set(cp, i);
	}
    update();
}

Corner_p  Face::C(int i) const {return _corns[(i+_size)%_size]; }
Vertex* Face::V(int i) const{return _corns[(i+_size)%_size]->V(); }

void Face::set(Corner_p c, int i){

	if (i<0){
		c->_f = this;
		return;
	}

	int ii = (i+_size)%_size;
	_corns[ii] = c; 
	c->_f = this; 
	c->set(ii);
}

void Face::set(Vertex* v, int i){
    v->set(C((i+_size)%_size));
}

void Face::update(bool links){

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
			_corns[i] = ci;
			ci = ci->next();
		}else 
			C(i-1)->setNext(_corns[i]);
		set(_corns[i], i);
	}
}

void Face::remove(){
	for(int i=0; i<size(); i++){
        Edge_p e = C(i)->E();
		if (!e)
			continue;
		if (e->isBorder()){
			mesh()->remove(e);
			//also remove V
		}else{
            Corner_p ctemp = C(i)->other();
			e->set(0, 1);
			e->set(ctemp, 0);
			delete C(i);
		}
	}
}

int Face::size() const {return _size;}


bool Edge::isBorder(){return !_c1 || !_c1->F();}

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

void Edge::remove(){

	if (!_c1)
		return;

    Face_p f1 = _c1->F();
    for(Corner_p c = _c1->next(); c!=_c1; c = c->next())
		_c0->F()->set(c, -1);
	_c0->F()->set(_c1, -1);

    Corner_p c0 = _c0;
    Corner_p c0n = _c0->next();
    Corner_p c0nn = _c0->next()->next();
	c0n->E()->set(_c1, c0n->isC0()?0:1);
	delete _c0->next();

    Corner_p c1n = _c1->next();
	c1n->E()->set(_c0, c1n->isC0()?0:1);
	_c0->setNext(_c1->next()->next());
	delete _c1->next();
	_c1->setNext(c0nn);

	c0->F()->set(c0);
    c0->F()->Face::update(true);
}

Edge::Edge(){
	_c0 = _c1 = 0;
    pData = 0;
}

Edge_p Mesh::insertEdge(Corner_p i_c0, Corner_p i_c1, bool updatefaces){

	if (i_c0->F() != i_c1->F())
		return 0;

    Corner_p c0n_0 = i_c0->next();
    Corner_p c1n_0 = i_c1->next();

    Face_p f0 = i_c0->F();
    Face_p f1 = addFace(4);

	f0->set(i_c0);
	f1->set(i_c1);

    Corner_p c0n = new Corner();
    f0->set(c0n,-1);
    i_c1->V()->set(c0n);

    Corner_p c1n = new Corner();
    f1->set(c1n,-1);
    i_c0->V()->set(c1n);

    i_c0->setNext(c0n);
    i_c1->setNext(c1n);
    c0n->setNext(c1n_0);
    c1n->setNext(c0n_0);

	//check edges
	if (i_c0->E()->_c0 == i_c0)
        i_c0->E()->set(c1n,0);
	else
        i_c0->E()->set(c1n,1);

	if (i_c1->E()->_c0 == i_c1)
        i_c1->E()->set(c0n,0);
	else
        i_c1->E()->set(c0n,1);

    for(Corner_p c = i_c1->next(); c!=i_c1; c = c->next())
		f1->set(c, -1);
	
	if (updatefaces){
        f0->Face::update(true);
        f1->Face::update(true);
	}

    Edge_p e = addEdge(i_c0, i_c1);

	return e;
}

