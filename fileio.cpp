#include <string>
#include "fileio.h"
#include "MeshShape/meshshape.h"

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
        outfile<<"sv "<<sv->P.x<<" "<<sv->P.y<<" "<<sv->N.x<<" "<<sv->N.y<<" "<<sv->N.z<<" "<<sv->id()<<"/"<<(sv->parent()?sv->parent()->id():0)<<endl;
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

    mesh->enamurateEdges();
    EdgeList edges = mesh->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        Edge_p pE = (*it);
        outfile<<"e "<<pE->pData->pSV[0]->id()<<" "<<pE->pData->pSV[1]->id()<<" "<<pE->pData->pSV[2]->id()<<" "<<pE->pData->pSV[3]->id()<<endl;
    }

    FaceList faces = mesh->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        Face_p pF = (*it);
        outfile<<"f "<<pF->C(0)->V()->pData->id()<<"/"<<pF->C(0)->E()->id()<<" "<<pF->C(1)->V()->pData->id()<<"/"<<pF->C(1)->E()->id()<<" "<<pF->C(2)->V()->pData->id()<<"/"<<pF->C(2)->E()->id()<<" "<<pF->C(3)->V()->pData->id()<<"/"<<pF->C(3)->E()->id()<<endl;
    }

    return true;
}


bool DefaultIO::read(Shape * pShape,  ifstream& infile){

    string line;
    char label[1];
    double px, py, nx, ny, nz;
    int id, parent;
    while(getline(infile, line)){
        if (line.empty())
            break;
        sscanf(line.c_str(),"%s %f %f %f %f %f %d/%d",label, &px, &py, &nx, &ny, &nz, &id, &parent);
        pShape->addVertex(Point(px, py));
        //ShapeVertex::get();
    }

    switch(pShape->type())
    {
    case MESH_SHAPE:
        read((MeshShape*)pShape, infile);
        break;

    case ELLIPSE_SHAPE:
        //write ellipse shape
        //write((EllipseShape*)pShape, outfile);
        break;

    }
}

bool DefaultIO::read(MeshShape * pMS, ifstream &infile){

}

Shape* DefaultIO::parseShape(const char* line){

    char label[1];
    double x, y;
    int type, id, parent;
    sscanf(line,"%s %f %f %d/%d/%d",&label, &type, &id, &parent);
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
