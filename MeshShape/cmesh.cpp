#include "cmesh.h"
#include "curvededge.h"

using namespace dlfl;

Mesh::Mesh(){
    resetCB();
}

void Mesh::resetCB(){
    _insertEdgeCB = 0;
    _splitEdgeCB  = 0;
    _unsplitEdgeCB  = 0;
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

Face_p Mesh::addFace(int s, bool isborder){
    Face_p f = new Face(s);
    f->_isBorder = isborder;
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

Face_p Mesh::addTriangle(Vertex* v0, Vertex* v1, Vertex* v2){

    Face_p f = addFace(3);
    f->set(v0, 0);
    f->set(v1, 1);
    f->set(v2, 2);
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

Face_p Mesh::addTriangle(Corner_p c0, Corner_p c1, Corner_p c2)
{
    Corner_p corns[3] = {c0, c1, c2};
    Edge_p e = 0;
    for(int i=0; i <3; i++){
        //if (corns[i]->next() != corns[(i+1)%3])
        if (!corns[i]->V()->to(corns[(i+1)%3]->V()))
            e = insertEdge(corns[i], corns[(i+1)%3]);
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

    if (c0 && _insertEdgeCB)
        _insertEdgeCB(e);

	return e;
}

void Mesh::remove(Face_p f, bool lazydel){
    /*if (_removeFaceCB)
        _removeFaceCB(f);*/

    addOperation(Operation::REMOVE_FACE, 0, 0, 0, f);
    f->remove(lazydel);
	if (lazydel){
		f->markDeleted();
	}else{
		_faces.remove(f);
		delete f;
	}
    //addOperation(Operation::REMOVE_FACE, 0, 0, 0, f);
}

void Mesh::remove(Edge_p e, bool lazydel){
    Corner_p c0 = e->C();
    removeEdge(c0, lazydel);
}

void Mesh::removeEdge(Corner_p c0, bool lazydel){

     Edge_p   e  = c0->E();
    Corner_p c1  = c0->other();
    Corner_p c0n = c0->next();
    Corner_p c1n = c1->next();

    if (c1!=c0n){
        c0n->E()->set(c1, c0n->isC1());
        c1->setNext(c0n->next());
        c0n->V()->set(c1);
    }else{ //dangling corner
        c1->setNext(c1);
    }

    if (c0!=c1n){
        c1n->E()->set(c0, c1n->isC1());
        c0->setNext(c1n->next());
        c1n->V()->set(c0);
    }else{ //dangling corner
        c0->setNext(c0);
    }

    Face_p f0 = c0->F(), f1 = c1->F();
    if (f0 == f1){
        f0 = addFace(1, f1->isBorder());
        f0->set(c1);
        f0->update(true);
    }else{
        f0->markDeleted();
    }

    f1->set(c0);
    f1->update(true);
    f1->_isBorder = f0->_isBorder | f1->_isBorder;

    addOperation(Operation::REMOVE_EDGE, c0, c1, e, f0);

    //clean up
    if (c0->F()->size()<2){
        c0->F()->markDeleted();
        c0->V()->markDeleted();
        c0->V()->set(0);
    }

    if (c1->F()->size()<2){
        c1->F()->markDeleted();
        c1->V()->markDeleted();
        c1->V()->set(0);
    }

    e->markDeleted();
    delete c0n;
    delete c1n;
}

Edge_p Mesh::insertEdge(Corner_p c0, Corner_p c1, bool updatefaces, Edge_p pE, Face_p pF){

    if (c0->E() && (c0->E() == c1->E()))
        return c0->E();

    bool issameface = c0->F() == c1->F();

    if (issameface)
        c0->F()->update(true);

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

    if (issameface){// need to split the face
        Face_p f0 = pF? pF : addFace(4);
        if (pF)
            pF->_isdeleted = false;

        int fsize = c0->F()->size();
        if (c0->I() > c1->I()){
            f0->set(c0->go(fsize - c0->I()-1));
            f0->update(true);
            c1->F()->update(true);
        }else{
            f0->set(c0->F()->C(0));
            c1->F()->set(c1->go(fsize - c1->I()-1));

            f0->update(true);
            c1->F()->update(true);
        }

    }else{ //combine faces
        c1->F()->_isdeleted = true; //????
        c0->F()->update(true);
    }

    if (!pE){
        pE = addEdge(c0, c1);
    }else{
        pE->set(c0, 0);
        pE->set(c1, 1);
        //pE->_isdeleted = true;
        if (_insertEdgeCB)
            _insertEdgeCB(pE);
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
    addOperation(Operation::SPLIT_EDGE, c0new, 0, c0new->prev()->E());

    if (_splitEdgeCB)
        _splitEdgeCB(c0new);

    return pC->next();
}

Corner_p Mesh::unsplitEdge(Corner_p pC){

    if (!pC)
        return 0;

    //Edge_p pE = pC->E();
    Corner_p c0 = pC;
    Corner_p c1 = pC->vNext();

    if (c0->isC1() && !c1->isC1()) //need swap
    {
        Corner_p tmp = c0;
        c0 = c1;
        c1 = tmp;
    }

    //c1->E()->_isdeleted = true;
    c0->E()->_isdeleted = true;
    c0->V()->_isdeleted = true;
    c1->E()->set(c1->prev(), c1->isC1());

    if (c0 == c0->F()->C(0)){
        c0->F()->set(c0->prev());
    }

    if (c1 == c1->F()->C(0)){
        c1->F()->set(c1->prev());
    }

    c0->prev()->setNext(c0->next());
    c1->prev()->setNext(c1->next());

    Corner_p c = c0->prev();

    c0->F()->update(true);
    c1->F()->update(true);

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

    Face_p pOutF = addFace(1, true);

    Corner_p * borderCorns = new Corner_p[size()*2];
    for(int i=0; i < size()*2; i++)
        borderCorns[i] = 0;

    for(std::list<Edge_p>::iterator it = _edges.begin(); it != _edges.end(); it++){
        Edge_p e = *it;
        if (!e->isBorder())
            continue;
        int v0 = e->C0()->V()->id();
        int v1 = e->C0()->next()->V()->id();
        Corner_p c1 = new Corner( e->C0()->next()->V(), pOutF);
        e->set(c1, 1);

        if (!pOutF->C()->E())
            pOutF->set(c1);

        if (borderCorns[v0]){
            c1->setNext(borderCorns[v0]);
        }else
            borderCorns[size()+v0] = c1;

        if (borderCorns[size()+v1])
            borderCorns[size()+v1]->setNext(c1);

        borderCorns[v1] = c1;
    }
    pOutF->update(true);

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

void Mesh::cleanUp(bool isenamurate){

    int id = 0;
    for(std::list<Vertex*>::iterator it = _verts.begin(); it!=_verts.end(); it++){
        Vertex_p pV = *it;
        if (isenamurate)
            pV->_id = id++;
        if (pV->_isdeleted){
            _verts.erase(it++);
            delete pV;
        }
    }

    id = 0;
    for(std::list<Edge*>::iterator it = _edges.begin(); it!=_edges.end(); it++){
        Edge_p pE = *it;
        if (isenamurate)
            pE->_id = id++;
        if (pE->_isdeleted){
            _edges.erase(it++);
            delete pE;
        }
    }

    id = 0;
    for(std::list<Face*>::iterator it = _faces.begin(); it!=_faces.end(); it++){
        Face_p pF = *it;
        if (isenamurate)
            pF->_id = id++;
        if (pF->_isdeleted){
            _faces.erase(it++);
            delete pF;
        }
    }
    _stack.clear();
}

void Mesh::addOperation(Operation::Type t, Corner_p c0, Corner_p c1, Edge_p e, Face_p f){
    if (!_isstackon)
        return;
    Operation op;
    op.type = t;
    if (c0)
        op.c0 = *c0;
    if (c1)
        op.c1 = *c1;
    op.pE = e;
    op.pED = e? e->pData : 0;

    op.pF = f;
    _stack.push_back(op);
}

int Mesh::rollback(int opid){

    _isstackon = false;

    int opcount = _stack.size() - opid;

    if (opcount<=0)
        return _stack.size();

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
            Corner_p c1 = op.c1.V()->find(op.c1.F());

            if (!c0)
                c0 = new Corner(op.c0.V());

            if (!c1)
                c1 = new Corner(op.c1.V());

            Edge_p e = addEdge(0);
            e->pData = op.pED;
            insertEdge(c0, c1, true, e, op.pF);
        }break;

        case Operation::SPLIT_EDGE:
        {
            Corner_p c = op.c0.V()->find(op.c0.F());
            c = unsplitEdge(c);
            if (op.pED)
                op.pED->setRef(c->E());
        }
            break;

        case Operation::REMOVE_FACE:
        {
            op.pF->_isdeleted = false; //?
            op.pF->_isBorder = false; //border edges cannot be removed
        }break;

        }
    }
    _isstackon = true;

    return _stack.size();
}


