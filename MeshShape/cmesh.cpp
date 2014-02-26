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
    _isstackon = true;
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

Face_p Mesh::addQuad(Vertex* v0, Vertex* v1, Vertex* v2, Vertex* v3, bool isedges){

    Face_p f = addFace(4);
	f->set(v0, 0);
	f->set(v1, 1);
	f->set(v2, 2);
	f->set(v3, 3);
	f->update();
    return f;
}

Face_p Mesh::addQuad(Corner_p c0, Corner_p c1, Corner_p c2, Corner_p c3)
{
    Corner_p corns[4] = {c0, c1, c2, c3};
    Edge_p e = 0;
    for(int i=0; i <4; i++){
        if (corns[i]->next() != corns[(i+1)%4])
            e = insertEdge(corns[i], corns[(i+1)%4]);
    }
    Face_p f =  e->C0()->F();
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
    /*if (_removeFaceCB)
        _removeFaceCB(f);*/

    f->remove(lazydel);
	if (lazydel){
		f->markDeleted();
	}else{
		_faces.remove(f);
		delete f;
	}
}
void Mesh::remove(Edge_p e, bool lazydel){
    /*if (_removeEdgeCB)
        _removeEdgeCB(e);*/
    Corner_p c0 = e->C(), c1 = c0->other();

    if (lazydel){
        e->markDeleted();
    }/*else{
        _edges.remove(e);
        delete e;
        e=0;
    }*/

    Corner_p c0n = c0->next();
    Corner_p c1n = c1->next();

    if (c0n == c1 && c1n==c0){
        addOperation(Operation::REMOVE_EDGE, c0, c1,e);
        c0->V()->markDeleted();
        c0->V()->set(0);
        c1->V()->markDeleted();
        c1->V()->set(0);
        delete c0;
        delete c1;
        return;
    }

    if (c0n == c1){
        c1n->E()->set(c0, c1n->isC1());
        c0->setNext(c1n->next());
        if (c0->F())
            c0->F()->set(c1n);

        addOperation(Operation::REMOVE_EDGE, c0, c1,e);
        //remove vertex
        c1->V()->markDeleted();
        c1->V()->set(0);
        c1n->V()->set(c0);

        delete c1n;
        delete c1;
        return;
    }

    if (c1n == c0){
        c0n->E()->set(c1, c0n->isC1());
        c1->setNext(c0n->next());
        if (c1->F())
            c1->F()->set(c0n);

        addOperation(Operation::REMOVE_EDGE, c0, c1,e);
        c0->V()->markDeleted();
        c0->V()->set(0);
        c0n->V()->set(c1);

        delete c0n;
        delete c0;
        return;
    }


    c0n->E()->set(c1, c0n->isC1());
    c1->setNext(c0n->next());
    c0n->V()->set(c1);
    delete c0n;

    c1n->E()->set(c0, c1n->isC1());
    c0->setNext(c1n->next());
    c1n->V()->set(c0);
    delete c1n;

    Face_p f0 = c0->F(), f1 = c1->F();
    if (!c0->isBorder() && c1->isBorder()){
        Face_p tmp = f0;
        f0 = f1;
        f1 = tmp;
    }

    if (f1){
        if (lazydel)
            f1->markDeleted();
        else
            delete f1;
    }

    if (f0)
    {
        f0->set(c0);
        f0->Face::update(true);
    }else{
        c1->_f = 0;
        for(Corner_p c = c1->next(); c!= c1; c = c->next())
            c->_f = 0;
    }

    addOperation(Operation::REMOVE_EDGE, c0, c1, e, f1);

}

Edge_p Mesh::insertEdge(Corner_p c0, Corner_p c1, bool updatefaces, Edge_p pE, Face_p pF){

    if (c0->E() == c1->E())
        return c0->E();

    Corner_p c0n_0 = c0->next();
    Corner_p c1n_0 = c1->next();

    Corner_p c0n = c1;
    if (c1n_0 && c1n_0 != c1){ //if not dangling vertex
        c0n = new Corner(c1->V());
        c0n->setNext(c1n_0);
    }
    c0->setNext(c0n);

    Corner_p c1n = c0;
    if (c0n_0 && c0n_0 != c0){ //if not dangling vertex
        c1n = new Corner(c0->V());
        c1n->setNext(c0n_0);
    }
    c1->setNext(c1n);

    //check edges
    if (c0->E())
        c0->E()->set(c1n, c0->isC1());

    if (c1->E())
        c1->E()->set(c0n, c1->isC1());

    Face_p f0 =  c0->F();
    if (c0->F() == c1->F()){
        if (pF){
            pF->_isdeleted = false;
            pF->_isBorder  = false;
            f0 = pF;
        }
        else
            f0 = addFace(4);
    }

    if (f0 && f0!=NULLFACE){

        f0->set(c0);
        f0->set(c0n,-1);
        if (updatefaces)
            f0->Face::update(true);
    }else if (updatefaces){
        //null face, to be fixed later
        c0->_f = 0;
        for(Corner_p c = c0->next(); c!=c0; c = c->next())
            c->_f = 0;
    }

    Face_p f1 = c1->F();
    if (f1 && f1!=NULLFACE){
        f1->set(c1);
        f1->set(c1n,-1);
        for(Corner_p c = c1->next(); c!=c1; c = c->next())
            f1->set(c, -1);
        if (updatefaces)
            f1->Face::update(true);

    }else if (updatefaces){
        //null face, to be fixed later
        c1->_f = 0;
        for(Corner_p c = c1->next(); c!=c1; c = c->next())
            c->_f = 0;
    }

    if (!pE){
        pE = addEdge(c0, c1);
    }else{
        pE->set(c0, 0);
        pE->set(c1, 1);
        pE->_isdeleted = true;
        /*if (_insertEdgeCB)
            _insertEdgeCB(pE);*/
    }

    addOperation(Operation::INSERT_EDGE, c0);

    return pE;
}


Corner_p Mesh::splitEdge(Corner_p pC, Vertex_p pV){

    if (!pC)
        return 0;

    Vertex* v = pV? pV : addVertex();

    Edge_p pE = pC->E();
    Corner_p c0 = pC->E()->C0();
    Corner_p c1 = pC->E()->C1();


    Corner_p c0new = 0;
    if (c0){
        c0new = new Corner(v);
        v->set(c0new);
        c0->F()->set(c0new, -1);

        Corner_p c0n = c0->next();
        c0->setNext(c0new);
        c0new->setNext(c0n);
    }

    Corner_p c1new=0;
    Corner_p c1old = c1;

    if (c1){
        c1new = new Corner(v);
        c1->F()->set(c1new, -1);

        Corner_p c1n = c1->next();
        c1->setNext(c1new);
        c1new->setNext(c1n);
        pE->set(c1new, 1);
    }

    addEdge(c0new, c1old);
    addOperation(Operation::SPLIT_EDGE, c0new);

    if (_splitEdgeCB)
        _splitEdgeCB(c0new);

    return pC->next();
}

Corner_p Mesh::mergeEdge(Corner_p pC){

    if (!pC)
        return 0;

    Edge_p pE = pC->E();
    Corner_p c0 = pC;
    Corner_p c1 = pC->vNext();

    if (c0->isC1() && !c1->isC1()) //need swap
    {
        Corner_p tmp = c0;
        c0 = c1;
        c1 = tmp;
    }

    c0->prev()->setNext(c0->next());
    c1->prev()->setNext(c1->next());
    c1->E()->set(c1, c1->isC1());

    Corner_p c = c0->prev();

    delete c0;
    delete c1;
    //remove e1 ?
    return c;
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

void Mesh::enamurateEdges(){
    int id = 0;
    for(std::list<Edge*>::iterator it = _edges.begin(); it!=_edges.end(); it++)
        (*it)->_id = id++;
}

void Mesh::enamurateFaces(){
    int id = 0;
    for(std::list<Face*>::iterator it = _faces.begin(); it!=_faces.end(); it++)
        (*it)->_id = id++;
}

void Mesh::buildEdges(bool isouterface){
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

    if (!isouterface)
        return;

    Corner_p * borderCorns = new Corner_p[size()*2];
    for(int i=0; i < size()*2; i++)
        borderCorns[i] = 0;

    for(std::list<Edge_p>::iterator it = _edges.begin(); it != _edges.end(); it++){
        Edge_p e = *it;
        if (!e->isBorder())
            continue;
        int v0 = e->C0()->V()->id();
        int v1 = e->C0()->next()->V()->id();
        Corner_p c1 = new Corner( e->C0()->next()->V(), 0);
        e->set(c1, 1);

        if (borderCorns[v0]){
            c1->setNext(borderCorns[v0]);
        }else
            borderCorns[size()+v0] = c1;

        if (borderCorns[size()+v1])
            borderCorns[size()+v1]->setNext(c1);

        borderCorns[v1] = c1;
    }

}

Mesh_p Mesh::deepCopy()
{

    Mesh_p pCopy = new Mesh();
    Vertex_p * vcopy = new Vertex_p[_verts.size()];
    int vid = 0;
    for(std::list<Vertex*>::iterator it = _verts.begin(); it!=_verts.end(); it++)
    {
        (*it)->_id = vid;
        vcopy[vid] = pCopy->addVertex((*it)->pData);
        vid++;
    }

    Edge_p * ecopy = new Edge_p[_edges.size()];
    for(int i = 0; i < _edges.size(); i++)
        ecopy[i] = 0;
    enamurateEdges();

    for(std::list<Face*>::iterator it = _faces.begin(); it!=_faces.end(); it++){
        Face_p f = (*it);
        Face_p fcopy = pCopy->addFace(f->size());

        for(int i=0; i<f->size(); i++){
            fcopy->set(vcopy[f->C(i)->V()->id()],i);
            Edge_p e = ecopy[f->C(i)->E()->id()];
            if (e){
                e->set(fcopy->C(i), fcopy->C(i)->isC1());
            }else{
                e = ecopy[f->C(i)->E()->id()] = pCopy->addEdge(fcopy->C(i));
                e->set(fcopy->C(i), fcopy->C(i)->isC1());
                e->pData = f->C(i)->E()->pData; //this may need a deep copy too!
            }

        }
    }

    return pCopy;
}

void Mesh::restore(FaceCache & face){
    face.restore();
    face.F()->_isdeleted = false;
}


void Mesh::ForAllEdges(void (*handler)(Edge_p),   bool isskipdeleted, bool isenamurate){
    int id = 0;
    for(std::list<Edge*>::iterator it = _edges.begin(); it!=_edges.end(); it++){
        if (isenamurate)
            (*it)->_id = id++;
        if (isskipdeleted && (*it)->_isdeleted)
            continue;
        handler(*it);
    }
}

void Mesh::ForAllFaces(void (*handler)(Face_p),   bool isskipdeleted, bool isenamurate){
    int id = 0;
    for(std::list<Face_p>::iterator it = _faces.begin(); it!=_faces.end(); it++){
        if (isenamurate)
            (*it)->_id = id++;
        if (isskipdeleted && (*it)->_isdeleted)
            continue;
        handler(*it);
    }
}

void Mesh::ForAllVerts(void (*handler)(Vertex_p), bool isskipdeleted, bool isenamurate){
    int id = 0;
    for(std::list<Vertex_p>::iterator it = _verts.begin(); it!=_verts.end(); it++){
        if (isenamurate)
            (*it)->_id = id++;
        if (isskipdeleted && (*it)->_isdeleted)
            continue;
        handler(*it);
    }
}

void Mesh::addOperation(Operation::Type t, Corner_p c0, Corner_p c1, Edge_p e, Face_p f){
    if (!_isstackon)
        return;
    Operation op;
    op.type = t;
    op.c0 = *c0;\
    if (c1)
        op.c1 = *c1;
    op.pE = e;
    op.pF = f;
    _stack.push_back(op);
}

int Mesh::rollback(int opid){

    _isstackon = false;

    int opcount = _stack.size()-opid;
    for(int i=0; i < opcount; i++){
        Operation op = _stack.back();
        _stack.pop_back();

        switch(op.type){

        case Operation::INSERT_EDGE:{
            remove(op.c0.E());
        }
            break;

        case Operation::REMOVE_EDGE:
        {

            op.c0.V()->_isdeleted = false;
            op.c1.V()->_isdeleted = false;

            Corner_p c0 = op.c0.V()->find(op.c0.F());
            Corner_p c1 = op.c1.V()->find(op.c0.F());

            if (!c0)
                c0 = new Corner(op.c0.V());

            if (!c1)
                c1 = new Corner(op.c1.V());

            insertEdge(c0, c1, true );//, 0, op.pF);
            //e->pData = op.pE->pData;
        }break;

        case Operation::SPLIT_EDGE:
        {
            Corner_p c = op.c0.V()->find(op.c0.F());
            mergeEdge(c);
        }
            break;

        }

        if (i==0) break;

    }
    _isstackon = true;

    return _stack.size();
}

Corner::Corner(Vertex_p v, Face_p f){
    if (v)
        v->set(this);
    _f = f;
    _e = 0;
    _i = -1;
    _next = _prev = 0;
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
    if (!_c)
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
    if (!_c)
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
    while(c->F()!= f){
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
    Corner_p c0 = C(0);
    for(Corner_p c = c0->next(); c!=c0; c = c->next())
        set(c, c->I() + off);
    set(c0, c0->I() + off);
}

void Face::remove(bool lazydel){ 
    EdgeList todel;
    for(int i=0; i<_size; i++){
        Corner_p ci = C(i);
        Edge_p e = ci->E();
        if (e->isBorder())
            todel.push_back(e);
    }

    int i=0;
    for(EdgeList::iterator it = todel.begin(); it!=todel.end(); it++){
        mesh()->remove(*it, lazydel);
    }

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
    return (i==-1)? (_c0->F()?_c0 : _c1) : ((i==0)?_c0:_c1);
}


Edge::Edge(){
	_c0 = _c1 = 0;
    pData = 0;
}



FaceCache::FaceCache(Face_p pF, bool isRemove){

    if (!pF){
        _size =  0;
        _corns = 0;
        _pF    = 0;
        return;
    }

    _isRemove = isRemove;
    _size  = pF->size();
    _corns = new Corner[_size];
    _pF = pF;
    _isC1 = 0;
    _isBorder =0;

    for(int i=0; i < _size; i++){
        _corns[i] = *pF->C(i);
        if (pF->C(i)->isC1()) _isC1 = _isC1 | (1 << i);
        if (pF->C(i)->E()->isBorder()) _isBorder = _isBorder | (1 << i);
    }
}

void FaceCache::restore(Face_p pF){
    if (!pF)
        pF = _pF;

    pF->init(_size);
    for(int i=0; i<_size; i++){
        Corner_p c  = pF->C(i);
        _corns[i].V()->_isdeleted;
        _corns[i].V()->set(c);

        bool isc1 = (_isC1 & (1<<i));
        Edge_p e = _corns[i].E();
        e->set(c, isc1);
        e->_isdeleted = false;
    }

    //now link borders //this part is tricky!!!
    for(int i=0; i<_size; i++){
        Corner_p c  = pF->C(i);
        bool isborder = (_isBorder & (1<<i));
        bool isc1 = (_isC1 & (1<<i));
        if (isborder){

        }
    }

}
