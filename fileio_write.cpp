#include <string>
#include <sstream>


#include "fileio.h"
#include "shape.h"
#include "mainwindow.h"
#include "ellipseshape.h"
#include "MeshShape/cmesh.h"
#include "MeshShape/meshshape.h"
#include "MeshShape/curvededge.h"
#include "MeshShape/Patch.h"
#include "canvas.h"

#ifndef MODELING_MODE
#include "Renderer/imageshape.h"
#include "Renderer/renderoptionspenal.h"
#endif

typedef std::vector<std::string>        StringVec;

//sscanf (sentence,"%s %*s %d",str,&i);
bool DefaultIO::save(const char *fname)
{

    ofstream outfile;
    outfile.open (fname);

    //if (Session::get()->canvas()->bgImage()->hasTexture())
    /*FOR_ALL_ITEMS(ShapeList, shapes){
        write((Shape*)(*it), outfile);
     }
    */

    ShapeList shapes = Session::get()->canvas()->shapes();
    for(ShapeList::reverse_iterator rit = shapes.rbegin(); rit != shapes.rend(); ++rit)
    {
        write((Shape*)(*rit), outfile);
    }

#ifndef MODELING_MODE

    double val[ROP_VALUES];
    Session::get()->mainWindow()->previewSettingsPanel->getValues(val);
    outfile<<"preview ";
    for(int i=0; i<ROP_VALUES; i++)
        outfile<<val[i]<<" ";
    outfile<<endl;

#endif

    outfile.close();
    return true;
}


bool DefaultIO::write(Shape * pShape, ofstream &outfile)
{
    writeShapeBase(pShape, outfile);
    SVList verts = pShape->getVertices();
    FOR_ALL_ITEMS(SVList, verts){
        //SKIP_DELETED_ITEM
        if ((*it)->isDeleted())
            continue;//outfile<<"##";
        writeShapeBase(*it, outfile);
    }

#ifndef MODELING_MODE

    double prev_val[PREV_PARAM];
    pShape->getPrevParam(prev_val);
    outfile<<"#preview ";
    for(int i = 0; i < PREV_PARAM; i++)
        outfile<<prev_val[i]<<" ";
    outfile<<endl;

#endif

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

void DefaultIO::writeShapeBase(ShapeBase_p pSB, ofstream& outfile)
{
    outfile <<"s "<<pSB->type();
    //this could be improved
    if (pSB->type() == SHAPE_VERTEX){
        ShapeVertex_p  pSV = (ShapeVertex_p)pSB;
        outfile<<" "<<pSV->name()<<"/"<<(pSV->parent()?pSV->parent()->name():0)<<"/"<<(pSV->pair()?pSV->pair()->name():0);
    }else{
        Shape_p pS = (Shape_p)pSB;
        outfile<<" "<<pS->name()<<"/"<<(pS->parent()?pS->parent()->name():0)<<"/0";
    }
    outfile<<" "<<((float)pSB->_P.x)<<" "<<(float)(pSB->_P.y);
    for(int c = 0; c < ACTIVE_CHANNELS; c++)
    {
        outfile<<" "<<((float)pSB->value[c].x)<<" "<<(float)(pSB->value[c].y)<<" "<<(float)(pSB->value[c].z);
    }
    outfile<<endl;
}

bool DefaultIO::writeMeshShape(MeshShape * pMS, ofstream& outfile)
{
    Mesh_p mesh = pMS->mesh();
    outfile<<"surface "<<pMS->_patchtype<<endl;

    mesh->enamurateFaces();
    FaceList faces = mesh->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        SKIP_DELETED_ITEM
        Face_p pF = (*it);
        if (pF->size()<2)
            continue;
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
