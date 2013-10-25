/********************************************************************

    Mesh.H	Header File

        A lite DLFL Mesh implementation
		Ozgur Gonen, May 2013
		Visualization Laboratory
		Texas A&M University

*********************************************************************/
#ifndef __Mesh_H__
#define __Mesh_H__

#include <list>

/*class Vec2;
class Vec3;
class Bezier;
class Patch;
*/
struct ShapeVertex;
struct EdgeData;
struct FaceData;

namespace dlfl {

typedef ShapeVertex VertexData;

class Corner;
class Edge;
class Vertex;
class Face;
class Mesh;

typedef Corner*     Corner_p;
typedef Edge*       Edge_p;
typedef Vertex*     Vertex_p;
typedef Face*       Face_p;
typedef Mesh*       Mesh_p;

typedef std::list<Edge_p>   EdgeList;
typedef std::list<Vertex_p> VertexList;
typedef std::list<Face_p>   FaceList;

class Mesh{

    EdgeList	_edges;
    VertexList	_verts;
    FaceList	_faces;

	//callbacks:
    void (*_insertEdgeCB)(Edge_p);
    void (*_splitEdgeCB)(Corner_p);
    void (*_removeEdgeCB)(Edge_p);

    void (*_addFaceCB)(Face_p);
    void (*_removeFaceCB)(Face_p);
    void* _caller;

    //friend Corner_p Edge::split(Corner_p f=0);


public:

    Mesh();

    EdgeList	edges(){return _edges;}
    VertexList	verts(){return _verts;}
    FaceList	faces(){return _faces;}
	
    inline int sizeE()const	{return _edges.size();}
    inline int sizeF()const {return _faces.size();}
    inline int size() const	{return _verts.size();}

    Vertex_p addVertex(VertexData* pData=0);

    Face_p addFace(int);
    Face_p addQuad(Vertex_p, Vertex_p, Vertex_p, Vertex_p); //strickly clock-wise
    Face_p addTriangle(Vertex_p, Vertex_p, Vertex_p);
    Edge_p addEdge(Corner_p c0, Corner_p c1=0);
    Edge_p insertEdge(Corner_p, Corner_p, bool updatefaces = true);
    Corner_p splitEdge(Edge_p, Vertex_p vP = 0, Face_p f = 0);

    void remove(Face_p f, bool lazydel = false);
    void remove(Edge_p e, bool lazydel = false);

	void updateF();
	void enamurateVerts();
	void buildEdges();

	//set callback functions
    void resetCB();
    void setInsertEdgeCB(void (*handler)(Edge_p)){_insertEdgeCB = handler;}
    void setSplitEdgeCB(void (*handler)(Corner_p)){_splitEdgeCB = handler;}
    void setRemoveEdgeCB(void (*handler)(Edge_p)){_removeEdgeCB = handler;}
    void setAddFaceCB(void (*handler)(Face_p)){	_addFaceCB = handler;}
    void setRemoveFaceCB(void (*handler)(Face_p)){_removeFaceCB = handler;}
    void setCaller(void* caller){ _caller = caller;}

    inline void* caller() const {return _caller;}
};

class Element{

	unsigned int _isdeleted;
	unsigned int _id;
    friend class Mesh;
    Mesh_p _mesh;

    void markDeleted(){_isdeleted = DELETED;}

public:

    Element(){ _mesh = 0; pStore =0; }

    inline int id() const {return _id;}
    Mesh* mesh()const {return _mesh;}
    bool isDeleted()const {return _isdeleted == DELETED;}

    const static unsigned int DELETED = 0x0AB10;

    void* pStore; //generic pointer for storage
};

class Edge: public Element{

    Edge();
	Corner	*_c0, *_c1;

	void remove();
    friend class Mesh;

public:

    inline Corner_p C0(){return _c0;}
    inline Corner_p C1(){return _c1;}
	
    Corner_p other(Corner_p c){return (c==_c0)?_c1:_c0;}
    void set(Corner_p, int i=-1);
	bool isBorder();

    EdgeData* pData;
};


class Face: public Element{

	int _size;
    Corner_p* _corns; //strickly cockwise

    Face(int s=4);
	void remove();

    friend class Mesh;

public:
	
    Corner_p  C(int i) const;
    Vertex_p  V(int i) const;
    inline int size() const;

    void set(Corner_p, int i=0);
    void set(Vertex_p, int i=0);

	void update(bool links=false);

    FaceData* pData;
};

class Vertex: public Element{

    Vertex(){
        _c = 0;
    }

    friend class Mesh;
    Corner_p _c;

public:

    VertexData* pData;
    //inline Point P(){return *pP;}

    void set(Corner_p);

    inline Corner_p C() const {return _c;}
    int edgeId(const Vertex_p v) const {return id() + v->id()*mesh()->size();}
    int edgeUId(const Vertex_p v) const {return (id() > v->id())?(v->id() + id()*mesh()->size()):(id() + v->id()*mesh()->size());}

};

class Corner{
	
	int _i;
	Corner *_prev, *_next;
    Edge_p _e;
    Face_p _f;
    Vertex_p _v;

    friend void Vertex::set(Corner_p);
    friend void Edge::set(Corner_p,int);
    friend void Face::set(Corner_p, int);
    friend void Face::set(Vertex_p, int);

public:

	Corner();

	inline int I(){return _i;}

    inline Edge_p	E() const {return _e;}
    inline Face_p	F() const {return _f;}
    inline Vertex_p V() const {return _v;}
    Corner_p next();
    Corner_p prev();

    Corner_p vNext();
    Corner_p vPrev();
    Corner_p other();

    void setNext(Corner_p c);
	void set(int i){_i = i;}

	bool isC0();
    bool isBorder();

};

}

#endif
