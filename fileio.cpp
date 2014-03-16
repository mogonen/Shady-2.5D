#include <string>
#include <sstream>

#include "fileio.h"
#include "shape.h"
#include "MeshShape/cmesh.h"
#include "MeshShape/meshshape.h"


struct SVLoad{
    ShapeVertex* sv;
    int parent_id, pair_id;
    SVLoad(ShapeVertex* sv_, int parent, int pair){
        sv = sv_;
        parent_id = parent;
        pair_id = pair;
    }
};

struct EdgeLoad{
    Edge_p e;
    int f0, c0, f1, c1;
    EdgeLoad(Edge_p e_, int f0_, int c0_, int f1_, int c1_ ){
        e = e_;
        f0 = f0_;
        c0 = c0_;
        f1 = f1_;
        c1 = c1_;
    }
};


typedef std::map<int, SVLoad*> SVLoadMap;
typedef std::map<int, Vertex_p> VertexLoadMap;
typedef std::map<int, Edge_p>   EdgeLoadMap;
typedef std::map<int, Face_p>   FaceLoadMap;
typedef std::vector<std::string> StringVec;

void split(const std::string &s, char delim, StringVec &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            elems.push_back(item);
    }
}

bool DefaultIO::save(const char *fname){

    ofstream outfile;
    outfile.open (fname);

    Shape* pShape = Session::get()->theShape();
    if (!pShape)
        return false;

    write((Shape*)pShape, outfile);

    outfile.close();
    return true;
}

bool DefaultIO::load(const char *fname){

    _infile.open (fname);
    std::string readline;
    bool isok = true;


    Shape* pShape = 0;
    ShapeList shapes;

    VertexLoadMap vertmap;
    FaceLoadMap   facemap;

    while(std::getline(_infile, readline))
    {
        if (readline.empty())
            continue;

        StringVec toks;
        std::string line = readline;
        split(readline, ' ', toks);
        std::string label = toks[0];

        if (label.compare("s")==0)
        {
            pShape = parseShape(line.c_str());
            if (!pShape){
                isok = false;
                break;
            }
            shapes.push_back(pShape);
            vertmap.clear();
            facemap.clear();
        } else if (pShape && label.compare("sv")==0)
        {
            float px, py, nx, ny, nz;
            int id, parent, pair;

            sscanf(toks[1].c_str(),"%d/%d/%d", &id, &parent, &pair);
            sscanf(toks[2].c_str(),"%f", &px);
            sscanf(toks[3].c_str(),"%f", &py);

            sscanf(toks[4].c_str(),"%f", &nx);
            sscanf(toks[5].c_str(),"%f", &ny);
            sscanf(toks[6].c_str(),"%f", &nz);

            ShapeVertex* sv = pShape->addVertex(Point(px, py));
            sv->pN()->set(nx, ny, nz);
            Load* load = new Load(sv);
            load->keys[Load::PARENT] = parent;
            load->keys[Load::PAIR] = pair;
            _loadmap[id] = load;

        }else if (pShape && label.compare("f")==0){

            Mesh_p pMesh = ((MeshShape*)(pShape))->mesh();

            int size = toks.size()-3;
            Face_p pF = pMesh->addFace(size, toks[size+2].compare("1")==0 );

            int fid;
            sscanf(toks[1].c_str(),"%d", &fid);
            facemap[fid] = pF;
            for(int i=0; i < size; i++){
                int svid;
                sscanf(toks[i+2].c_str(),"%d", &svid);
                ShapeVertex_p sv = (ShapeVertex_p)_loadmap[svid]->pObj;
                VertexLoadMap::iterator it = vertmap.find(svid);
                Vertex_p pV = 0;
                if (it == vertmap.end()){//create new vertex
                    pV = ((MeshShape*)(pShape))->addMeshVertex(sv);
                    vertmap[svid] = pV;
                }else
                    pV = it->second;

                pV->set(pF->C(i));
            }
            pF->update();

        }else if (pShape && label.compare("e")==0)
        {
            Mesh_p pMesh = ((MeshShape*)(pShape))->mesh();

            int c0f, c0i,c1f, c1i, sv1, sv2;
            sscanf(toks[1].c_str(), "%d/%d", &c0f, &c0i);
            sscanf(toks[2].c_str(), "%d/%d", &c1f, &c1i);
            sscanf(toks[3].c_str(), "%d", &sv1);
            sscanf(toks[4].c_str(), "%d", &sv2);

            Edge_p pE = pMesh->addEdge(0);
            pE->set(facemap[c0f]->C(c0i), 0);
            pE->set(facemap[c1f]->C(c1i), 1);
            pE->pData = new EdgeData(pE);

            if (sv1 && sv2)
                pE->pData->initCurve((ShapeVertex_p)_loadmap[sv1]->pObj, (ShapeVertex_p)_loadmap[sv2]->pObj);
            else
                pE->pData->initCurve(0,0);

        }
    }
    _infile.close();

    FOR_ALL_ITEMS(LoadMap, _loadmap){
        //int id = it->first;
        Load* load = it->second;
        if (load->keys[Load::PARENT])
        {
            ((Draggable_p)(_loadmap[load->keys[Load::PARENT]]->pObj))->adopt((Draggable_p)load->pObj);
        }

        if (load->keys[Load::PAIR])
        {
           ((ShapeVertex_p)(_loadmap[load->keys[Load::PAIR]]->pObj))->setPair((ShapeVertex_p)load->pObj);
        }
    }

    FOR_ALL_ITEMS(ShapeList, shapes)
    {
        Shape* pShape = (*it);
        pShape->update();
        Session::get()->insertShape(pShape);
    }
    return isok;
}


bool DefaultIO::read(Shape * pShape){

    switch(pShape->type())
    {
    case MESH_SHAPE:
        parseMeshShape((MeshShape*)pShape);
        break;

    case ELLIPSE_SHAPE:
        //write ellipse shape
        //write((EllipseShape*)pShape, outfile);
        break;

    }
}


bool DefaultIO::parseMeshShape(MeshShape * pMS){

    Mesh_p pMesh = pMS->mesh();

    std::string line;
    VertexLoadMap vertmap;
    FaceLoadMap facemap;

    while(std::getline(_infile, line)){

        if (line.empty())
            break;

        StringVec toks;
        split(line, ' ', toks);

        int isf = toks[0].compare("f");
        int ise = toks[0].compare("e");

        if (isf == 0){

            int size = toks.size()-3;
            Face_p pF = pMesh->addFace(size, toks[size+2].compare("1")==0 );
            int fid;
            sscanf(toks[1].c_str(),"%d", &fid);
            facemap[fid] = pF;
            for(int i=0; i < size; i++){
                int svid;
                sscanf(toks[i+2].c_str(),"%d", &svid);
                ShapeVertex_p sv = (ShapeVertex_p)_loadmap[svid]->pObj;
                VertexLoadMap::iterator it = vertmap.find(svid);
                Vertex_p pV = 0;
                if (it == vertmap.end()){//create new vertex
                    pV = pMesh->addVertex(sv);
                    vertmap[svid] = pV;
                }else
                    pV = it->second;

                pV->set(pF->C(i));
            }
            pF->update();
        }

        if (ise == 0)
        {
            int c0f, c0i,c1f, c1i, sv1, sv2;
            sscanf(toks[1].c_str(),"%d/%d", &c0f, &c0i);
            sscanf(toks[2].c_str(),"%d/%d", &c1f, &c1i);

            Edge_p pE = pMesh->addEdge(0);//facemap[c0f]->C(c0i), facemap[c1f]->C(c1i));
            pE->set(facemap[c0f]->C(c0i), 0);
            pE->set(facemap[c1f]->C(c1i), 1);
            /*pE->pData = new EdgeData(pE);
            if (sv1 && sv2)
                pE->pData->initCurve(svmap[sv1]->sv, svmap[sv2]->sv);
            else
                pE->pData->initCurve(0,0);*/
        }
    }    

}

Shape* DefaultIO::parseShape(const char* line){

    char label[1];
    float x, y;
    int type, id, parent;
    sscanf(line,"%s %f %f %d/%d/%d",&label, &x, &y, &type, &id, &parent);
    if (label[0] != 's')
        return 0;

    Shape* pShape = 0;
    switch(type){

    case MESH_SHAPE:
        pShape = new MeshShape();
        break;

    case ELLIPSE_SHAPE:
        //pShape = new EllipseShape();
        break;

    }

    pShape->translate(Vec2(x,y));
    return pShape;
}








/* WRITE STUFF*/







//sscanf (sentence,"%s %*s %d",str,&i);
bool DefaultIO::write(Shape * pShape, ofstream &outfile){

    outfile <<"s "<<pShape->P().x<<" "<< pShape->P().y<<" "<<pShape->type()<<"/"<<pShape->name()<<"/"<<(pShape->parent()?pShape->parent()->name():0)<<endl;
    SVList verts = pShape->getVertices();
    FOR_ALL_ITEMS(SVList, verts){
        ShapeVertex_p sv = (*it);
        outfile<<"sv "<<sv->id()<<"/"<<(sv->parent()?((ShapeVertex_p)sv->parent())->id():0)<<"/"<<(sv->pair()?sv->pair()->id():0)<<" "<<sv->P().x<<" "<<sv->P().y<<" "<<sv->N().x<<" "<<sv->N().y<<" "<<sv->N().z<<" "<<endl;
    }

    switch(pShape->type())
    {
    case MESH_SHAPE:
        writeMeshShape((MeshShape*)pShape, outfile);
        break;

    case ELLIPSE_SHAPE:
        //write ellipse shape
        //write((EllipseShape*)pShape, outfile);
        break;

    }
}

bool DefaultIO::writeMeshShape(MeshShape * pMS, ofstream& outfile){

    Mesh_p mesh = pMS->mesh();
    /*VertexList verts = mesh->verts();
    FOR_ALL_ITEMS(VertexList, verts){
        Vertex_p pV = (*it);
        outfile<<"v "<<pV->id()<<"/"<<pV->pData->id()<<endl;
    }*/

    outfile<<"#shapedata"<<endl;

    mesh->enamurateFaces();
    FaceList faces = mesh->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        SKIP_DELETED_ITEM
        Face_p pF = (*it);
        outfile<<"f "<<pF->id();
        for(int i=0; i<pF->size(); i++)
            outfile<<" "<<pF->C(i)->V()->pData->id();
        outfile<<" "<<pF->isBorder();
        outfile<<endl;
    }

    mesh->enamurateEdges();
    EdgeList edges = mesh->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        SKIP_DELETED_ITEM
        Edge_p pE = (*it);
        //<<pE->C0()-F()-id()<<"/"<<pE->C0()->I()<<" "
        if (pE->pData->pCurve->count()==2)
            outfile<<"e "<<pE->C0()->F()->id()<<"/"<<pE->C0()->I()<<" "<<pE->C1()->F()->id()<<"/"<<pE->C1()->I()<<endl;
        else
            outfile<<"e "<<pE->C0()->F()->id()<<"/"<<pE->C0()->I()<<" "<<pE->C1()->F()->id()<<"/"<<pE->C1()->I()<<" "<<pE->pData->pSV[1]->id()<<" "<<pE->pData->pSV[2]->id()<<endl;
    }

    return true;
}


/*
        if (toks[0].compare("e") == 0)
        {
            int c0f, c0i,c1f, c1i, sv1, sv2;
            sscanf(line.c_str(),"%s %d/%d %d/%d %d %d", label, &c0f, &c0i, &c1f, &c1i, &sv1, &sv2);
            Edge_p pE = pMesh->addEdge(0);//facemap[c0f]->C(c0i), facemap[c1f]->C(c1i));
            //pE->set(facemap[c0f]->C(c0i), 0);
            //pE->set(facemap[c1f]->C(c1i), 1);
            /*pE->pData = new EdgeData(pE);
            if (sv1 && sv2)
                pE->pData->initCurve(svmap[sv1]->sv, svmap[sv2]->sv);
            else
                pE->pData->initCurve(0,0);
            edgecount++;


*/

/* Edge_p pE = pMesh->addEdge(0);
 pE->set(facemap[0]->C(0), 0);
 pE->set(facemap[4]->C(7), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[0]->C(1), 0);
 pE->set(facemap[1]->C(3), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[0]->C(2), 0);
 pE->set(facemap[2]->C(0), 1);


 pE = pMesh->addEdge(0);
 pE->set(facemap[0]->C(3), 0);
 pE->set(facemap[4]->C(0), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[1]->C(0), 0);
 pE->set(facemap[4]->C(6), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[1]->C(1), 0);
 pE->set(facemap[4]->C(5), 1);


 pE = pMesh->addEdge(0);
 pE->set(facemap[1]->C(2), 0);
 pE->set(facemap[3]->C(0), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[2]->C(1), 0);
 pE->set(facemap[3]->C(3), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[2]->C(2), 0);
 pE->set(facemap[4]->C(2), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[2]->C(3), 0);
 pE->set(facemap[4]->C(1), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[3]->C(1), 0);
 pE->set(facemap[4]->C(4), 1);

 pE = pMesh->addEdge(0);
 pE->set(facemap[3]->C(2), 0);
 pE->set(facemap[4]->C(3), 1);*/
