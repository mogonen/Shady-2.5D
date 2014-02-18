#include <qgl.h>
#include <QDebug>
#include "SampleShape.h"
#include "meshshape/spineshape.h"
#include "meshshape/meshshape.h"
#include "MeshShape/meshcommands.h"
#include "glwidget.h"

#ifdef FACIAL_SHAPE
#include "FacialShape/facialshape.h"
#endif

#include "meshshape/Patch.h"
#include "controlpoint.h"
#include "curve.h"
#include "shapecontrol.h"
#include "ellipseshape.h"
#include "canvas.h"

#include "Renderer/imageshape.h"

#define theSHAPE Session::get()->theShape()

//selection stuff
bool isInRenderMode(){
    GLint mode;
    glGetIntegerv(GL_RENDER_MODE, &mode);
    return ( mode == GL_RENDER);
}

bool isSelectMode(MeshOperation::SelectMode eMode){

    if (isInRenderMode() || Session::isRender(DRAG_ON))
        return false;

    MeshOperation* pMO = dynamic_cast<MeshOperation*>(Session::get()->theCommand());
    if (!pMO)
        return false;
   return pMO->getSelectMode() == eMode;
}

void Selectable::render(int mode)
{
    if (!isInRenderMode())
        glLoadName(name());
}

Selectable_p select(GLint hits, GLuint *buff){
   //only one selectable object in the stack for now
   Selectable_p pSel = Session::get()->selectionMan()->get(buff[3]);
   /*if ( name & ( 1 << UI_BIT ) )
   {
      pSel =Session::get()->controller();
     Session::get()->controller()->startSelect(name);
   }
   else*/
   return pSel;
}

inline bool selectionColor(Selectable_p pSel){
    if (pSel->isTheSelected()){
        glColor3f(1.0, 0.1, 0);
        return true;
    }
    else if (pSel->isInSelection())
    {
        glColor3f(1.0, 1.0, 0);
        return true;
    }
    return false;
}
// now all renders here

void SampleShape::render(int mode) {

    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    /*for(int i = 0; i < 4; i++)
        glVertex3f(p[i].x, _p[i].y, 0);*/
    glEnd();
}

void Light::render(int mode) {
    Selectable::render(mode);

    glColor3f(1.0, 1.0, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex3f(P().x, P().y, 0.001);
    glEnd();
}

void ControlPoint::render(int mode) {

    Selectable::render(mode);

    if (!Session::isRender(NORMALS_ON) && isChild() && !isActive())
        return;

    //glColor3f(1.0, 1.0, 1.0);
    glColor3f(_color[0], _color[1], _color[2]);
    selectionColor((Selectable_p)this);

    glPointSize(5.0);
    glBegin(GL_POINTS);
    glVertex3f(P().x, P().y, 0);
    glEnd();

    if (isChild() && isInRenderMode()){
        glBegin(GL_LINES);
        Point p0 = P();
        Point p1 = parent()->P();
        glVertex3f(p0.x, p0.y, 0);
        glVertex3f(p1.x, p1.y, 0);
        glEnd();
    }

}

void Shape::render(int mode)
{
    if(Session::isRender(DRAG_ON) && this == theSHAPE)
       Session::get()->controller()->renderControls((Shape_p)this);

    Selectable::render(mode);
    ensureUpToDate();
}

void ShapeControl::renderControls(Shape_p shape)
{
    if (_theHandler->isActive())
            return;

    SVList verts = shape->getVertices();
    FOR_ALL_CONST_ITEMS(SVList, verts){
        SKIP_DELETED_ITEM
        if (Session::isRender(NORMALS_ON) && (*it)->isNormalControl)
                (*it)->pControlN()->render();

        (*it)->render();
    }
  /*
    //could not get pushname popname working!
    glPointSize(4.0);
    SVList verts = shape->getVertices();
    FOR_ALL_CONST_ITEMS(SVList, verts){
        SKIP_DELETED_ITEM
        ShapeVertex_p sv = (*it);
        if (sv->parent() && (!_theSelected || (_theSelected != sv->parent() && _theSelected->parent() != sv->parent())) )
            continue;

        Point p0 = sv->P();

        unsigned int svname = sv->id() | (1 << UI_BIT);
        if (Session::isRender(NORMALS_ON) && sv->isNormalControl){
            unsigned int svn_name = svname | (1 << NORMAL_CONTROL_BIT);
            Point p1 = p0 + Point(sv->N()*NORMAL_RAD);
            glLoadName(svn_name);
            glBegin(GL_POINTS);
            glColor3f(1.0, 0, 0);
            glVertex3f(p1.x, p1.y, 0);
            glEnd();

            if (isInRenderMode()){
                //glColor3f(1.0, 1.0, 1.0);
                glColor3f(0.0, 0.0, 0.0);
                glBegin(GL_LINES);
                glVertex3f(p0.x, p0.y, 0);
                glVertex3f(p1.x, p1.y, 0);
                glEnd();
            }

        }

        glLoadName(svname);
        glBegin(GL_POINTS);
        //glColor3f(1.0, 1.0, 1.0);
        glColor3f(0.0, 0.0, 0.0);
        if(sv->flag ==1)
            glColor3f(1.0, 1.0, 0);
        glVertex3f(p0.x, p0.y, 0);
        glEnd();

        if (isInRenderMode() && sv->parent()){
            glBegin(GL_LINES);
            glVertex3f(p0.x, p0.y, 0);
            glVertex3f(sv->parent()->P().x, sv->parent()->P().y, 0);
            glEnd();
        }
    }*/
}

void SpineShape::render(int mode){

    if(isInRenderMode()){
        glPointSize(5.0);
        glBegin(GL_POINTS);
            FOR_ALL_CONST_ITEMS(SVertexList, _verts)
            {
                if (!(*it)->pSV)
                    continue;
                Point p = (*it)->P();
                glVertex2f(p.x, p.y);
            }
        glEnd();
    }

    glBegin(GL_LINES);
        FOR_ALL_CONST_ITEMS(SVertexList, _verts)
        {
            if (!(*it)->pSV)
                continue;

            Point p0 = (*it)->P();
            for(SVertexList::iterator itv = (*it)->links.begin(); itv != (*it)->links.end(); itv++)
            {
                Point p1 = (*itv)->P();
                glVertex3f(p0.x, p0.y, 0);
                glVertex3f(p1.x, p1.y, 0);
            }
        }
    glEnd();
}

void renderEdge(Edge_p pEdge){

    if (pEdge->pData->pCurve){
        pEdge->pData->pCurve->render();
        return;
    }
    //non selectable line representation
    glColor3f(1.0,1.0, 0);
    Point p0 = P0(pEdge);
    Point p1 = P1(pEdge);

    glBegin(GL_LINES);
    glVertex3f(p0.x, p0.y, 0);
    glVertex3f(p1.x, p1.y, 0);
    glEnd();
}

void renderFace(Face_p pFace){
    if (!pFace->pData->pSurface)
        return;

   /* if(mode&BRIGHT_MODE)
        glColor3f(diffuse.redF()*2.0,diffuse.greenF()*2.0,diffuse.blueF()*2.0);
    else if (mode&DARK_MODE)
        glColor3f(1-diffuse.redF(),1-diffuse.greenF(),1-diffuse.blueF());
    else
        glColor3f(diffuse.redF(),diffuse.greenF(),diffuse.blueF());
   */

    pFace->pData->pSurface->render();
}

void MeshShape::render(int mode) {

    if ( this == theSHAPE &&( isInRenderMode() || isSelectMode(MeshOperation::EDGE))  )
    {
       _control->ForAllEdges(renderEdge);
    }

    //too messy, fix it!
    if (isInRenderMode() || isSelectMode(MeshOperation::FACE) || Session::isRender(DRAG_ON) )
    {
         _control->ForAllFaces(renderFace);
    }
}

#ifdef FACIAL_SHAPE
void FacialShape::initBG(){
    QImage img_data = QGLWidget::convertToGLFormat(QImage(QString::fromStdString(m_imgName)));
    glGenTextures(1, &tt);
    glBindTexture(GL_TEXTURE_2D, tt);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}
void FacialShape::render() const{
    MeshShape::render();
//    if(texture[0])
//    {
//        glEnable(GL_TEXTURE_2D);
//        glBegin(GL_QUADS);
//        glColor4f(1.0, 1.0, 1.0, 1.0);
//        glTexCoord2d(0.0,0.0);
//        glVertex3f(-1.0,-1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(0.0,1.0);
//        glVertex3f(-1.0,1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(1.0,1.0);
//        glVertex3f(1.0,1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(1.0,0.0);
//        glVertex3f(1.0,-1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glEnd();
//        glDisable(GL_TEXTURE_2D);
//    }
}
#endif

void Curve::render(int mode) {

    Selectable::render();
    //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0, 0, 0);
    selectionColor((Selectable_p)this);
    if (isTheSelected())
        glColor3f(1.0, 0, 0);

    glBegin(_isClosed? GL_LINE_LOOP : GL_LINE_STRIP);
    FOR_ALL_I(_size){
        Point p = this->P(i);
        glVertex3f(p.x, p.y, 0);
    }
    glEnd();
}


void Patch4::render(int mode){

    if (!Session::isRender(DRAG_ON))
      Selectable::render(mode);

      for(int j=0; j < Ni; j++){
        for(int i = 0; i< Ni; i++){

            Point p[4];
            p[0] = P(i, j);
            p[1] = P(i+1, j);
            p[2] = P(i+1, j+1);
            p[3] = P(i, j+1);

            Vec3 n[4];
            n[0] = _ns[ind(i, j)];
            n[1] = _ns[ind(i+1, j)];
            n[2] = _ns[ind(i+1, j+1)];
            n[3] = _ns[ind(i, j+1)];

            if (isInSelection() || !isInRenderMode())
            {
                selectionColor((Selectable_p)this);
                glBegin(GL_POLYGON);
                for(int k=0; k<4; k++){
                    glVertex3f(p[k].x, p[k].y, 0);
                }
                glEnd();
                continue;
            }

            if (Session::isRender(WIREFRAME_ON)){

                if (Session::isRender(NORMALS_ON))
                {
                    //drawNormals
                    glColor3f(1,1,1);
                    glPointSize(4.0);

                    Vec3 p0 = p[0];
                    Vec3 p1 = p0 + n[0]*NORMAL_RAD;
                    glBegin(GL_LINES);
                    glVertex3f(p0.x, p0.y, p0.z);
                    glVertex3f(p1.x, p1.y, p1.z);
                    glEnd();

                    glBegin(GL_POINTS);
                    glVertex3f(p0.x, p0.y, p0.z);
                    glEnd();
                }

                glColor3f(1.0,1.0,1.0);
                glLineWidth(0.5);
                glBegin(GL_LINE_LOOP);
                for(int k=0; k<4; k++)
                    glVertex3f(p[k].x, p[k].y, 0);
                glEnd();

            }

            glBegin(GL_POLYGON);
            for(int k = 0; k < 4; k++){
               if (Session::isRender(SHADING_ON) && !Session::isRender(PREVIEW_ON))
                    glNormal3f(n[k].x, n[k].y, n[k].z );
                else
                   glColor3f((n[k].x+1)/2, (n[k].y+1)/2, 1.0);

                glVertex2f(p[k].x, p[k].y);
                glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();
        }
    }

}


void EllipseShape::render(int mode) {

    FOR_ALL_J(_segV){
        FOR_ALL_I(_segU){

            Point p[4];
            p[0] = _ps[ind(i,j)];
            p[1] = _ps[ind(i+1, j)];
            p[2] = _ps[ind(i+1, j+1)];
            p[3] = _ps[ind(i, j+1)];

            Normal n[4];
            n[0] = _ns[ind(i, j)];
            n[1] = _ns[ind(i+1, j)];
            n[2] = _ns[ind(i+1, j+1)];
            n[3] = _ns[ind(i, j+1)];

            int size = 4;
            if (j == _segV-1){
                size = 3;
                p[2].set();
                n[2].set(0,0,1);
            }

            if (Session::isRender(WIREFRAME_ON)){

                if (Session::isRender(NORMALS_ON))
                {
                    //drawNormals
                    glColor3f(1,1,1);
                    glPointSize(4.0);

                    Vec3 p0 = p[0];
                    Vec3 p1 = p0 + n[0]*NORMAL_RAD;
                    glBegin(GL_LINES);
                    glVertex3f(p0.x, p0.y, p0.z);
                    glVertex3f(p1.x, p1.y, p1.z);
                    glEnd();

                    glBegin(GL_POINTS);
                    glVertex3f(p0.x, p0.y, p0.z);
                    glEnd();
                }

                glColor3f(1.0,1.0,1.0);
                glLineWidth(0.5);
                glBegin(GL_LINE_LOOP);
                for(int k=0; k<size; k++)
                    glVertex3f(p[k].x, p[k].y, 0);
                glEnd();

            }

            glBegin(GL_POLYGON);
            for(int k = 0; k < size; k++){
               if (Session::isRender(SHADING_ON) && !Session::isRender(PREVIEW_ON))
                    glNormal3f(n[k].x, n[k].y, n[k].z );
                else
                   glColor3f((n[k].x+1)/2, (n[k].y+1)/2, 1.0);

                glVertex2f(p[k].x, p[k].y);
                glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();

        }
    }
}

void TransformHandler::render(){

    if (!_pShape || _pShape!=theSHAPE)
        return;

    _handles[0]->render();
    _handles[1]->render();
    //_rotHandle->renderNamed();

    if (isInRenderMode()){
        glColor3f(1.0, 1.0, 1.);
        glBegin(GL_LINE_LOOP);
        glVertex2f(_bbox.P[0].x, _bbox.P[0].y);
        glVertex2f(_bbox.P[0].x, _bbox.P[1].y);
        glVertex2f(_bbox.P[1].x, _bbox.P[1].y);
        glVertex2f(_bbox.P[1].x, _bbox.P[0].y);
        glEnd();
    }
}


void ImageShape::InitializeTex()
{
    QImage loadedImage;
    if(m_texUpdate&UPDATE_SM)
    {
        if(glIsTexture(m_texSM))
            Session::get()->glWidget()->deleteTexture(m_texSM);
        if(m_SMFile.isEmpty()||!loadedImage.load(m_SMFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(0.0,0.0,255.0,255.0));
            _shaderParam.m_averageNormal = QVector2D(0.0,0.0);
        }
        else
        {
            int w = loadedImage.width();
            int h = loadedImage.height();
            if(w>h)
                m_height = m_width*h/w;
            else
                m_width = m_height*w/h;
            m_SMimg = loadedImage;
            calAverageNormal();
        }
        m_texSM = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
        if(GetPenal())
            GetPenal()->SetNewSize(m_width,m_height);
    }

    if(m_texUpdate&UPDATE_DARK)
    {
        if(glIsTexture(m_texDark))
            Session::get()->glWidget()->deleteTexture(m_texDark);
        if(m_DarkFile.isEmpty()||!loadedImage.load(m_DarkFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(0.0,0.0,0.0,255.0));
        }
        m_texDark = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }

    if(m_texUpdate&UPDATE_BRIGHT)
    {
        if(glIsTexture(m_texBright))
            Session::get()->glWidget()->deleteTexture(m_texBright);
        if(m_BrightFile.isEmpty()||!loadedImage.load(m_BrightFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(255.0,255.0,255.0,255.0));
        }
        m_texBright = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }
    m_texUpdate = NO_UPDATE;
}


void ImageShape::render(int mode)
{

    if(m_texUpdate!=NO_UPDATE)
        InitializeTex();
    if( mode < SM_MODE)
    {
        switch(m_curTexture)
        {
        case 0:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            break;
        case 1:
            glBindTexture(GL_TEXTURE_2D, m_texDark);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, m_texBright);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            mode = mode|LABELDEPTH_MODE;
            break;
        }
    }
    else
        if(mode&SM_MODE||mode&LABELDEPTH_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texSM);
        else if(mode&DARK_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texDark);
        else if(mode&BRIGHT_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texBright);


    Session::get()->glWidget()->getMShader()->bind();
    Session::get()->glWidget()->getMShader()->setUniformValue("alpha_th", (float)m_alpha_th);
    if(mode&LABELDEPTH_MODE)
        Session::get()->glWidget()->getMShader()->setUniformValue("isLabelDepth", (float)1.0);
    else
        Session::get()->glWidget()->getMShader()->setUniformValue("isLabelDepth", (float)0.0);
//    Session::get()->glWidget()->getMShader()->setUniformValue("label_depth", (float)m_alpha_th);

    int total_num = Session::get()->canvas()->getNumShapes();
    qDebug()<<"normal"<<_shaderParam.m_averageNormal;
//    glDepthMask(GL_FALSE);
    double delta_LB2RT = -(_shaderParam.m_averageNormal.x()*m_width+_shaderParam.m_averageNormal.y()*m_height)*m_stretch;
    double delta_LT2BR = -(_shaderParam.m_averageNormal.x()*m_width-_shaderParam.m_averageNormal.y()*m_height)*m_stretch;
    double center_depth = this->m_assignedDepth;//(float)(_shaderParam.m_layerLabel+1)/(total_num+1);

    qDebug()<<"_layerLabel"<<(int)_shaderParam.m_layerLabel;
    qDebug()<<"total_num"<<(int)total_num;
    qDebug()<<"center_depth"<<center_depth;
    qDebug()<<"delta_LB2RT"<<delta_LB2RT;
    qDebug()<<"delta_LT2BR"<<delta_LT2BR;


//    if(_shaderParam.m_averageNormal.z()<0)
//    {
//        delta_LB2RT =-delta_LB2RT;
//        delta_LT2BR =-delta_LT2BR;
//    }


    //1.1 is for numerical issue

    float blf = (center_depth-delta_LB2RT);
    blf = CapValue(blf,0,1);
    float tlf = (center_depth-delta_LT2BR);
    tlf = CapValue(tlf,0,1);
    float trf = (center_depth+delta_LB2RT);
    trf = CapValue(trf,0,1);
    float brf = (center_depth+delta_LT2BR);
    brf = CapValue(brf,0,1);

    glBegin(GL_QUADS);
    glColor4f(center_depth,blf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-m_width,-m_height,blf);
    QVector3D bl(-m_width,-m_height,blf);
//    glVertex3f(-m_width,-m_height,0.5);

    glColor4f(center_depth,tlf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-m_width,m_height,tlf);
    QVector3D tl(-m_width,m_height,tlf);
//    glVertex3f(-m_width,m_height,0.5);

    glColor4f(center_depth,trf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0,1.0);
    glVertex3f(m_width,m_height,trf);
    QVector3D tr(m_width,m_height,trf);

//    glVertex3f(m_width,m_height,0.5);

    glColor4f(center_depth,brf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0,0.0);
    glVertex3f(m_width,-m_height,brf);
    QVector3D br(m_width,-m_height,brf);
    glEnd();

    QVector3D n1 = QVector3D::crossProduct(br-bl,tl-bl);
//    QVector3D n2 = QVector3D::crossProduct(tr-br,tr-tl);
//    if(n1.z()>0)
//        _shaderParam.m_trueNormal = n1.normalized();
//    else
//        _shaderParam.m_trueNormal = -n1.normalized();

    qDebug()<<"bl"<<blf<<"tl"<<tlf<<"tr"<<trf<<"br"<<brf;
    qDebug()<<"bl"<<bl<<"tl"<<tl<<"tr"<<tr<<"br"<<br;

    //    qDebug()<<"n1"<<n1;
//    qDebug()<<"n2"<<n2;

//    glVertex3f(m_width,-m_height,0.5);

//    glDepthMask(GL_TRUE);
    Session::get()->glWidget()->getMShader()->release();
    glDisable(GL_TEXTURE_2D);
    _shaderParam.m_trueNormal = QVector3D(n1.x(),n1.y(), n1.z()).normalized();
    _shaderParam.m_centerDepth = QVector3D(m_width+P().x,-m_height+P().y,brf);
    _shaderParam.m_boundingbox[0] = -m_width+P().x;
    _shaderParam.m_boundingbox[1] = m_width+P().x;
    _shaderParam.m_boundingbox[2] = -m_height+P().y;
    _shaderParam.m_boundingbox[3] = m_height+P().y;
    _shaderParam.m_shadowcreator = this->m_shadowCreator;
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glBegin(GL_POINTS);
//    glColor3f(1.0,1.0,0.0);
//    glVertex3f(_shaderParam.m_centerDepth.x(),_shaderParam.m_centerDepth.y(),0.0);
//    glEnd();

}

float ImageShape::CapValue(float in_num, float low_cap, float high_cap)
{
    if(in_num<low_cap)
        return low_cap;
    if(in_num>high_cap)
        return high_cap;
    return in_num;
}
