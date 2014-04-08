#include <string>
#include <sstream>

#include "fileio.h"
#include "shape.h"
#include "ellipseshape.h"
#include "MeshShape/cmesh.h"
#include "MeshShape/meshshape.h"
#include "MeshShape/curvededge.h"
#include "canvas.h"

#ifndef MODELING_MODE
#include "Renderer/imageshape.h"
#endif

typedef std::vector<std::string>        StringVec;

//sscanf (sentence,"%s %*s %d",str,&i);
bool DefaultIO::save(const char *fname)
{

    ofstream outfile;
    outfile.open (fname);

    ShapeList shapes = Session::get()->canvas()->shapes();
    FOR_ALL_ITEMS(ShapeList, shapes){
        write((Shape*)(*it), outfile);
    }

    outfile.close();
    return true;
}


bool DefaultIO::write(Shape * pShape, ofstream &outfile)
{

    outfile <<"s "<<pShape->P().x<<" "<< pShape->P().y<<" "<<pShape->type()<<"/"<<pShape->name()<<"/"<<(pShape->parent()?pShape->parent()->name():0)<<endl;
    SVList verts = pShape->getVertices();
    FOR_ALL_ITEMS(SVList, verts){
        ShapeVertex_p sv = (*it);
        outfile<<"sv "<<sv->id()<<"/"<<(sv->parent()?((ShapeVertex_p)sv->parent())->id():0)<<"/"<<(sv->pair()?sv->pair()->id():0)<<" "<<sv->P().x<<" "<<sv->P().y<<" "<<sv->N().x<<" "<<sv->N().y<<" "<<sv->N().z<<" "<<endl;
    }
    outfile<<"#shapedata "<<pShape->name()<<endl;

    switch(pShape->type())
    {
    case MESH_SHAPE:
        writeMeshShape((MeshShape*)pShape, outfile);
        break;

    case ELLIPSE_SHAPE:
        writeEllipseShape((EllipseShape*)pShape, outfile);
        break;

    case IMAGE_SHAPE:
        writeImageShape((ImageShape*)pShape, outfile);
        break;

    }

    outfile<<endl;
}

bool DefaultIO::writeMeshShape(MeshShape * pMS, ofstream& outfile){

    Mesh_p mesh = pMS->mesh();

    /*
    VertexList verts = mesh->verts();
    FOR_ALL_ITEMS(VertexList, verts){
        Vertex_p pV = (*it);
        outfile<<"v "<<pV->id()<<"/"<<pV->pData->id()<<endl;
    }
    */

    mesh->enamurateFaces();
    FaceList faces = mesh->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        SKIP_DELETED_ITEM
        Face_p pF = (*it);
        outfile<<"f "<<pF->id();
        for(int i=0; i<pF->size(); i++){
            outfile<<" "<<pF->C(i)->V()->pData->id();
        }
        outfile<<" "<<pF->isBorder();
        outfile<<endl;
    }

    mesh->enamurateEdges();
    EdgeList edges = mesh->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        SKIP_DELETED_ITEM
        Edge_p pE = (*it);
        //<<pE->C0()-F()-id()<<"/"<<pE->C0()->I()<<" "
        /*if (pE->pData->pCurve->count()==2)
            outfile<<"e "<<pE->C0()->F()->id()<<"/"<<pE->C0()->I()<<" "<<pE->C1()->F()->id()<<"/"<<pE->C1()->I()<<endl;
        else*/
            outfile<<"e "<<pE->C0()->F()->id()<<"/"<<pE->C0()->I()<<" "<<pE->C1()->F()->id()<<"/"<<pE->C1()->I()<<" "<<pE->pData->getTangentSV(0)->id()<<" "<<pE->pData->getTangentSV(1)->id()<<endl;
    }
    return true;
}

bool DefaultIO::writeEllipseShape(EllipseShape * pIS, ofstream& outfile)
{
    double rx, ry;
    pIS->getRad(rx, ry);
    outfile<<"rad "<<(float)rx<<" "<<(float)ry<<endl;
}

bool DefaultIO::writeImageShape(ImageShape * pIS, ofstream& outfile){
#ifndef MODELING_MODE
/*
    double                      m_alpha_th;
    double                      m_stretch;
    double                      m_assignedDepth;
    double                      m_width;
    double                      m_height;

    int                         m_texUpdate;
    bool                        m_shadowCreator;
    GLuint                      m_texSM;
    GLuint                      m_texDark;
    GLuint                      m_texBright;
    GLuint                      m_texDisp;

    QString                     m_SMFile;
    QString                     m_DarkFile;
    QString                     m_BrightFile;
    QString                     m_DispFile;
*/
    outfile<<(float)pIS->m_alpha_th<<", "<<(float)pIS->m_stretch<<", "<<(float)pIS->m_assignedDepth<<", "<<(float)pIS->m_width<<", "<<(float)pIS->m_height<<endl;
    outfile<<pIS->m_texUpdate<<", "<<pIS->m_shadowCreator<<", "<<pIS->m_texSM<<", "<<pIS->m_texDark<<", "<<pIS->m_texBright<<", "<<pIS->m_texDisp<<endl;

    for(int channel = 0; channel<ACTIVE_CHANNELS; channel++ ){
        if (!pIS->m_fileName[channel].isEmpty())
            outfile<<qPrintable(pIS->m_fileName[channel])<<endl;
    }

#endif
}
