#include <string>
#include <sstream>

#include "fileio.h"
#include "shape.h"
#include "MeshShape/cmesh.h"
#include "MeshShape/meshshape.h"


typedef std::vector<std::string> StringVec;

StringVec &split(const std::string &s, char delim, StringVec &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            elems.push_back(item);
    }
    return elems;
}


StringVec split(const std::string &s, char delim) {
    StringVec elems;
    split(s, delim, elems);
    return elems;
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

    ifstream infile;
    infile.open (fname);
    string line;
    while(getline(infile, line)){
        Shape* pShape =  parseShape(line.c_str());
        if (!pShape)
            return false;
        read(pShape, infile);
    }

    infile.close();
    return true;
}
//sscanf (sentence,"%s %*s %d",str,&i);
bool DefaultIO::write(Shape * pShape, ofstream &outfile){

    outfile <<"s "<<pShape->P().x<<" "<< pShape->P().y<<" "<<pShape->type()<<"/"<<pShape->name()<<"/"<<(pShape->parent()?pShape->parent()->name():0)<<endl;
    SVList verts = pShape->getVertices();
    FOR_ALL_ITEMS(SVList, verts){
        ShapeVertex_p sv = (*it);
        outfile<<"sv "<<sv->P().x<<" "<<sv->P().y<<" "<<sv->N().x<<" "<<sv->N().y<<" "<<sv->N().z<<" "<<sv->id()<<"/"<<(sv->parent()?((ShapeVertex_p)sv->parent())->id():0)<<"/"<<(sv->pair()?sv->pair()->id():0)<<endl;
    }
    switch(pShape->type())
    {
    case MESH_SHAPE:
        write((MeshShape*)pShape, outfile);
        break;

    case ELLIPSE_SHAPE:
        //write ellipse shape
        //write((EllipseShape*)pShape, outfile);
        break;

    }
}

bool DefaultIO::write(MeshShape * pMS, ofstream& outfile){

    Mesh_p mesh = pMS->mesh();
    /*VertexList verts = mesh->verts();
    FOR_ALL_ITEMS(VertexList, verts){
        Vertex_p pV = (*it);
        outfile<<"v "<<pV->id()<<"/"<<pV->pData->id()<<endl;
    }*/

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

bool DefaultIO::read(Shape * pShape,  ifstream& infile){

    string line;
    char label[1];
    float px, py, nx, ny, nz;
    int id, parent, pair;
    SVLoadMap svmap;
    while(getline(infile, line)){
        if (line.empty())
            break;
        sscanf(line.c_str(),"%s %f %f %f %f %f %d/%d/%d",label, &px, &py, &nx, &ny, &nz, &id, &parent, &pair);
        ShapeVertex* sv = pShape->addVertex(Point(px, py));
        sv->pN()->set(nx, ny, nz);
        svmap[id] = new SVLoad(sv, parent, pair);
    }

    FOR_ALL_ITEMS(SVLoadMap, svmap){
        //int id = it->first;
        SVLoad* svload = it->second;
        if (svload->parent_id){
            svmap[svload->parent_id]->sv->adopt(svload->sv);
        }

        if (svload->pair_id){
            svload->sv->setPair(svmap[svload->pair_id]->sv);
        }
    }

    switch(pShape->type())
    {
    case MESH_SHAPE:
        read((MeshShape*)pShape, infile, svmap);
        break;

    case ELLIPSE_SHAPE:
        //write ellipse shape
        //write((EllipseShape*)pShape, outfile);
        break;

    }
}

typedef std::map<int, Vertex_p> VertexLoadMap;
typedef std::map<int, Edge_p>   EdgeLoadMap;
typedef std::map<int, Face_p>   FaceLoadMap;

bool DefaultIO::read(MeshShape * pMS, ifstream &infile, SVLoadMap& svmap){

    Mesh_p pMesh = pMS->mesh();

    string line;
    char label[1];

    VertexLoadMap vertmap;
    FaceLoadMap facemap;

    int edgecount = 0;

    while(getline(infile, line)){

        if (line.empty())
            break;

        StringVec toks = split(line, ' ');

        if (toks[0].compare("e") == 0)
        {
            int c0f, c0i,c1f, c1i, sv1, sv2;
            sscanf(line.c_str(),"%s %d/%d %d/%d %d %d", label, &c0f, &c0i, &c1f, &c1i, &sv1, &sv2);
            Edge_p pE = pMesh->addEdge(0);//facemap[c0f]->C(c0i), facemap[c1f]->C(c1i));
            pE->set(facemap[c0f]->C(c0i), 0);
            pE->set(facemap[c1f]->C(c1i), 1);
            pE->pData = new EdgeData(pE);
            if (sv1 && sv2)
                pE->pData->initCurve(svmap[sv1]->sv, svmap[sv2]->sv);
            else
                pE->pData->initCurve(0,0);
            edgecount++;
        }

        if (toks[0].compare("f") == 0){

            int size = toks.size()-3;
            Face_p pF = pMesh->addFace(size, toks[size+2].compare("1")==0 );
            int fid;
            sscanf(toks[1].c_str(),"%d", &fid);
            facemap[fid] = pF;
            for(int i=0; i < size; i++){
                int svid;
                sscanf(toks[i+2].c_str(),"%d", &svid);
                ShapeVertex_p sv = svmap[svid]->sv;
                VertexLoadMap::iterator it = vertmap.find(svid);
                Vertex_p pV =0;
                if (it == vertmap.end()){//create new vertex
                    pV = pMesh->addVertex(sv);
                    vertmap[svid] = pV;
                }else
                    pV = it->second;

                pV->set(pF->C(i));
            }
            pF->update();
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

