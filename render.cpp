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
#include "Renderer/layernormalcontrol.h"

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
   Selectable_p pSel = Session::get()->selectionMan()->get(buff[3]);
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

void SampleShape::render(int mode)
{

    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    /*for(int i = 0; i < 4; i++)
        glVertex3f(p[i].x, _p[i].y, 0);*/
    glEnd();
}

void ControlPoint::render(int mode){

    Selectable::render(mode);

    if (!Session::isRender(NORMALS_ON) && isChild() && !isActive())
        return;

    //glColor3f(1.0, 1.0, 1.0);
    glColor3f(_color[0], _color[1], _color[2]);
    selectionColor((Selectable_p)this);

    glPointSize(5.0);
    glBegin(GL_POINTS);
    Point p = P();
    glVertex3f(p.x, p.y, 0);
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
    if(Session::isRender(DRAG_ON) && this == theSHAPE && !Session::isRender(PREVIEW_ON))
       Session::get()->controller()->renderControls((Shape_p)this);

    Selectable::render(mode);
    ensureUpToDate();
}

void ShapeControl::renderControls(Shape_p shape)
{
    if (_theHandler->isActive())
            return;

    if(Session::isRender(NORMALS_ON))
        shape->pControlN()->render();

    SVList verts = shape->getVertices();
    FOR_ALL_CONST_ITEMS(SVList, verts)
    {
        SKIP_DELETED_ITEM
        ShapeVertex_p sv = *it;

        if (sv->ref() && sv->ref()->isDeleted())
            continue;

        if (Session::isRender(NORMALS_ON) && sv->isNormalControl)
                (sv)->pControlN()->render();

        sv->render();
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
        glColor3f(0,0,0);
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
        glColor3f(0,0,0);
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

void renderEdge(Edge_p pE){

 #ifdef SHOW_DLFL
    //render corners
    if (isInRenderMode()){
        Corner_p c0 = pE->C();
        Corner_p c1 = c0->other();

        Vec2 tan = P1(c0) - P0(c0);
        Vec3 n = Vec3(tan)%Vec3(0,0,1);
        Vec2 n2d(n.x, n.y);

        Point pc00, pc01, pc10, pc11;

        pc00 = P0(c0) + n2d*0.1 + tan*0.05;
        pc01 = P1(c0) + n2d*0.1 - tan*0.25;

        pc10 = P0(c1) - n2d*0.1 - tan*0.05;
        pc11 = P1(c1) - n2d*0.1 + tan*0.25;

        glBegin(GL_LINES);

        if (c0->isBorder())
            glColor3f(1.0, 0, 0);
        else
            glColor3f(0, 1.0, 0);

        glVertex2f(pc00.x, pc00.y);
        glVertex2f(pc01.x, pc01.y);

        if (c1->isBorder())
            glColor3f(1.0, 0, 0);
        else
            glColor3f(0, 1.0, 0);

        glVertex2f(pc10.x, pc10.y);
        glVertex2f(pc11.x, pc11.y);
        glEnd();


        glPointSize(2.0);
        glBegin(GL_POINTS);
        if (c0->isC1())
            glColor3f(1.0, 0, 0);
        else
            glColor3f(0,1.0,0);

        if (c0->V()->C()->V() != c0->V())
            glColor3f(0,0,0);

        glVertex2f(pc00.x, pc00.y);

        if (c1->isC1())
            glColor3f(1.0, 0, 0);
        else
            glColor3f(0,1.0,0);
        if (c1->V()->C()->V() != c1->V())
            glColor3f(0.0,0,0);

        glVertex2f(pc10.x, pc10.y);
        glEnd();

        if (c0->I() == 0 ){
            glPointSize(4.0);
            glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 0);
            glVertex2f(pc00.x, pc00.y);
            glEnd();
        }
        if (c1->I() == 0 ){
            glPointSize(4.0);
            glBegin(GL_POINTS);
            glColor3f(1.0, 1.0, 0);
            glVertex2f(pc10.x, pc10.y);
            glEnd();
        }
    }
#endif

    if (pE->pData){
        pE->pData->render();
        return;
    }

    //non selectable line representation
    glColor3f(1.0,1.0, 0);
    Point p0 = P0(pE);
    Point p1 = P1(pE);

    glBegin(GL_LINES);
    glVertex3f(p0.x, p0.y, 0);
    glVertex3f(p1.x, p1.y, 0);
    glEnd();
}

void renderFace(Face_p pFace)
{
    if (!pFace->pData)
        return;

    pFace->pData->render();
}

void MeshShape::render(int mode)
{

    if (Session::selectMode() == SELECT_VERTEX || Session::selectMode() == SELECT_TANGENT || Session::selectMode() == SELECT_VERTEX_TANGENT)
    {
        SVList verts = getVertices();
        FOR_ALL_CONST_ITEMS(SVList, verts)
        {
            SKIP_DELETED_ITEM
            ShapeVertex_p sv = *it;
            if (sv->ref() && sv->ref()->isDeleted())
                continue;

            if (Session::selectMode() == SELECT_VERTEX && sv->parent())
                continue;

            if (Session::selectMode() == SELECT_TANGENT && ! sv->parent())
                continue;

            sv->render();
        }
    }

    //fix this!!!
    if (( (this == theSHAPE || !Session::isRender(SURFACES_ON)) &&( isInRenderMode() || isSelectMode(MeshOperation::EDGE)) || Session::selectMode() == SELECT_EDGE) && Session::isRender(CURVES_ON))
    {
       _control->ForAllEdges(renderEdge);
    }

    //too messy, fix it!
    if ((isInRenderMode() || isSelectMode(MeshOperation::FACE) || Session::isRender(DRAG_ON) || Session::selectMode() == SELECT_SHAPE ) && Session::isRender(SURFACES_ON))
    {
        if (Session::isRender(PREVIEW_ON))
        {
            if (mode&BRIGHT_MODE)
                glColor3f(0.9, 0.8, 0.8); //glColor3f(diffuse.redF(), diffuse.greenF(), diffuse.blueF());
            else if (mode&DARK_MODE)
                glColor3f(0.1, 0.1, 0.1); //glColor3f(diffuse.redF()*0.1, diffuse.greenF()*0.1, diffuse.blueF()*0.1);
        }else if (Session::isRender(SHADING_ON)){

#ifdef  RENDERING_MODE
            RGB diff = value[BRIGHT_CHANNEL];
            RGB amb  = value[DARK_CHANNEL];

            GLfloat mat_diff[] = { diff.x, diff.y, diff.z, 1.0 };
            GLfloat mat_amb[] = { amb.x, amb.y, amb.z, 1.0 };

            //GLfloat mat_shininess[] = { 50.0 };
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_amb);
#endif
            //glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
        }
        //_control->ForAllFaces(renderFace);
        FaceList faces = _control->faces();
        FOR_ALL_ITEMS(FaceList, faces)
        {
            SKIP_DELETED_ITEM;
            if ((*it)->pData)
                (*it)->pData->render(mode);
        }
    }


}

void Patch4::render(int mode){

    if (!Session::isRender(DRAG_ON) && Session::selectMode() != SELECT_SHAPE)
        Selectable::render(mode);

      for(int j=0; j < _sampleVi; j++){
        for(int i = 0; i< _sampleUi; i++)
        {
            Point p[4];
            p[0] = P(i, j);
            p[1] = P(i+1, j);
            p[2] = P(i+1, j+1);
            p[3] = P(i, j+1);

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

            if (Session::isRender(WIREFRAME_ON))
            {
                if (Session::isRender(NORMALS_ON))
                {
                    //drawNormals
                    glColor3f(1,1,1);
                    glPointSize(4.0);

                    Vec3 n = _maps[NORMAL_CHANNEL][ind(i, j)];

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

            bool isGLShading = (Session::channel() == GL_SHADING) && !Session::isRender(PREVIEW_ON);
            bool isNormalChannel =  (Session::channel() == NORMAL_CHANNEL && !mode) || (mode&SM_MODE);

            if (isGLShading)
                    glEnable(GL_LIGHTING);

            glBegin(GL_POLYGON);            
            for(int k = 0; k < 4; k++)
            {
                int index = ind(i + (k==1 || k==2), j + (k>1));
                int channel = Session::channel();

                channel = mode&DARK_MODE ? DARK_CHANNEL : (mode&BRIGHT_MODE? BRIGHT_CHANNEL : (mode&SM_MODE?NORMAL_CHANNEL : ((mode&LABELDEPTH_MODE)?DEPTH_CHANNEL:channel)) );
                RGB val = _maps[(channel<ACTIVE_CHANNELS)?channel:(ACTIVE_CHANNELS-1)][index];

                if (isGLShading)
                {
                    RGB diff = _maps[BRIGHT_CHANNEL][index];
                    RGB amb  = _maps[DARK_CHANNEL][index];
                    GLfloat mat_diff[] = {diff.x, diff.y, diff.z, 1.0}; //diff.w};
                    GLfloat mat_amb[]  = {amb.x, amb.y, amb.z, 1.0}; //amb.w};

                    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
                    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);

                    Vec3 n =  _maps[NORMAL_CHANNEL][index];
                    glNormal3f(n.x, n.y, n.z );
                }
                else if (isNormalChannel)
                {                    
                    glColor4f((val.x+1)/2, (val.y+1)/2, 1.0, 1.0);
                }else{
                    glColor4f(val.x, val.y, val.z, 1.0); //col[k].w);
                }

                glVertex2f(p[k].x, p[k].y);
                glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();

            if (isGLShading)
                    glDisable(GL_LIGHTING);

        }
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


void CurvedEdge::render(int mode) {

    Selectable::render();
    //glColor3f(1.0, 1.0, 1.0);
    glColor3f(0, 0, 0);
    selectionColor((Selectable_p)this);
    if (isTheSelected())
        glColor3f(1.0, 0, 0);

    glBegin(GL_LINE_STRIP);
    FOR_ALL_I(size()){
        Point p = P(i);
        glVertex3f(p.x, p.y, 0);
    }
    glEnd();
}

void Rectangle::render(int mode){

    if (!Session::isRender(DRAG_ON))
      Selectable::render(mode);

    /*Point p[4] = {P0(C(0)), P0(C(1)), P0(C(2)), P0(C(3))};
    //Point n[4] = {C(0)->V()-, P0(C(1)), P0(C(2)), P0(C(3))};
    glColor3f(0.75, 0.75, 0.75);
    selectionColor((Selectable_p)this);
    glBegin(GL_POLYGON);
    for(int k = 0; k < 4; k++)
    {
        glVertex2f(p[k].x, p[k].y);
    }*/

    glColor3f(0.75, 0.75, 0.75);
    glBegin(GL_POLYGON);
    for(int k = 0; k < _pFace->size(); k++)
    {
        Point p = P0(_pFace->C(k));
        glVertex2f(p.x, p.y);
    }
    glEnd();

}

void EllipseShape::render(int mode){

    bool isGLShading = (Session::channel() == GL_SHADING) && !Session::isRender(PREVIEW_ON);
    //bool isNormalChannel =  (Session::channel() == NORMAL_CHANNEL && !mode) || (mode&SM_MODE);
    RGB bright = value[BRIGHT_CHANNEL];
    RGB dark  = value[DARK_CHANNEL];
    RGB depth  = value[DEPTH_CHANNEL];

    RGB col = value[Session::channel()<ACTIVE_CHANNELS ? Session::channel() : (ACTIVE_CHANNELS-1) ];

#ifdef RENDERING_MODE
    if (isGLShading)
    {

        GLfloat mat_diff[] = { bright.x, bright.y, bright.z, 1.0 };
        GLfloat mat_amb[] = { dark.x, dark.y, dark.z, 1.0 };

        //GLfloat mat_shininess[] = { 50.0 };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diff);
        glMaterialfv(GL_FRONT, GL_AMBIENT, mat_amb);
    }
#endif

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

            if (isGLShading)
            {
                glEnable(GL_LIGHTING);
            }

            glBegin(GL_POLYGON);
            for(int k = 0; k < size; k++)
            {
                //verry messy, fix this!!!

                if (isGLShading)
                {
                    glNormal3f(n[k].x, n[k].y, n[k].z );
                }
                else if (mode)
                {
                    if (mode&DARK_MODE)
                        glColor3f(dark.x, dark.y, dark.z);
                    else if (mode&BRIGHT_MODE)
                        glColor3f(bright.x, bright.y, bright.z);
                    else if (mode&LABELDEPTH_MODE) {
                        glColor3f(depth.x, depth.y, depth.z);
                    }
                    else if (mode&SM_MODE) {
                        glColor3f((n[k].x+1)/2, (n[k].y+1)/2, 1.0);
                    }
                }else{
                    if (Session::channel() == NORMAL_CHANNEL)
                    {
                        glColor3f((n[k].x+1)/2, (n[k].y+1)/2, 1.0);
                    }
                    else
                        glColor3f(col.x, col.y, col.z);
                }

                glVertex2f(p[k].x, p[k].y);
                glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();

            if (isGLShading)
                    glDisable(GL_LIGHTING);

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

#ifndef MODELING_MODE
void ImageShape::InitializeTex()
{
    QImage loadedImage;
    if(m_texUpdate&UPDATE_SM)
    {
        if(glIsTexture(m_texSM))
            Session::get()->glWidget()->deleteTexture(m_texSM);
        if(m_fileName[NORMAL_CHANNEL].isEmpty()||!loadedImage.load(m_fileName[NORMAL_CHANNEL]))
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
            //calAverageNormal();
        }
        m_texSM = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
        if(GetPenal())
            GetPenal()->SetNewSize(m_width,m_height);
    }

    if(m_texUpdate&UPDATE_DARK)
    {
        if(glIsTexture(m_texDark))
            Session::get()->glWidget()->deleteTexture(m_texDark);
        if(m_fileName[DARK_CHANNEL].isEmpty()||!loadedImage.load(m_fileName[DARK_CHANNEL]))
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
        if(m_fileName[BRIGHT_CHANNEL].isEmpty()||!loadedImage.load(m_fileName[BRIGHT_CHANNEL]))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(255.0,255.0,255.0,255.0));
        }
        m_texBright = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }
/*
    if(m_texUpdate&UPDATE_DISP)
    {
        if(glIsTexture(m_texDisp))
            Session::get()->glWidget()->deleteTexture(m_texDisp);
        //needs to be replaced
        if(m_fileName[DEPTH_CHANNEL].isEmpty()||!loadedImage.load(m_fileName[DEPTH_CHANNEL]))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(0.0,0.0,0.0,0.0));
        }
        m_texDisp = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }
*/
    m_texUpdate = NO_UPDATE;
}

void ImageShape::render(int mode)
{
    Shape::render(mode);
    if(m_texUpdate!=NO_UPDATE)
        InitializeTex();

    Session::get()->glWidget()->glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texSM);
    glDisable(GL_TEXTURE0);
    Session::get()->glWidget()->glActiveTexture(GL_TEXTURE0+1);
    if(mode&DEFAULT_MODE||mode&DRAG_MODE&&!(mode&SM_MODE||mode&DARK_MODE||mode&BRIGHT_MODE||mode&LABELDEPTH_MODE))
    {
        int channel = Session::get()->channel();
        switch(channel)//m_curTexture)
        {
        case NORMAL_CHANNEL:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            break;
        case DARK_CHANNEL:
            glBindTexture(GL_TEXTURE_2D, m_texDark);
            break;
        case BRIGHT_CHANNEL:
            glBindTexture(GL_TEXTURE_2D, m_texBright);
            break;
        case DEPTH_CHANNEL:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            mode = mode|LABELDEPTH_MODE;
        case DISP_CHANNEL:
            glBindTexture(GL_TEXTURE_2D, m_texDisp);
            break;
        }
    }
    else
    {
        if(mode&SM_MODE||mode&LABELDEPTH_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texSM);
        else if(mode&DARK_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texDark);
        else if(mode&BRIGHT_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texBright);
        else if(mode&DISPLACE_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texDisp);
    }
    glDisable(GL_TEXTURE0+1);

    Session::get()->glWidget()->glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE0);


    Session::get()->glWidget()->getMShader()->bind();
    Session::get()->glWidget()->getMShader()->setUniformValue("alpha_th", (float)m_alpha_th);
    Session::get()->glWidget()->getMShader()->setUniformValue("texSM", 0);
    Session::get()->glWidget()->getMShader()->setUniformValue("tex", 1);

    if(mode&LABELDEPTH_MODE)
        Session::get()->glWidget()->getMShader()->setUniformValue("isLabelDepth", (float)1.0);
    else
        Session::get()->glWidget()->getMShader()->setUniformValue("isLabelDepth", (float)0.0);


    Vec3 n = value[NORMAL_CHANNEL];
    QVector3D layerNormal(0, 0, 1.0);//layerNormal(n.x, n.y, n.z);// = _NormalControl->Normal3D();
    qDebug()<<layerNormal;
    double delta_LB2RT;
    double delta_LT2BR;
    if(layerNormal.z()>0.1)
    {
        delta_LB2RT = -(layerNormal.x()*m_width+layerNormal.y()*m_height)/layerNormal.z();
        delta_LT2BR = -(layerNormal.x()*m_width-layerNormal.y()*m_height)/layerNormal.z();
    }
    else
    {
        delta_LB2RT = 1;
        delta_LT2BR = 1;
    }

    double center_depth = this->m_assignedDepth;


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
    glTexCoord2d(0.0, 0.0);
    glVertex3f(-m_width,-m_height,blf);
    QVector3D bl(-m_width,-m_height,blf);
//    glVertex3f(-m_width,-m_height,0.5);

    glColor4f(center_depth,tlf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(0.0, 1.0);
    glVertex3f(-m_width,m_height,tlf);
    QVector3D tl(-m_width,m_height,tlf);
//    glVertex3f(-m_width,m_height,0.5);

    glColor4f(center_depth,trf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0, 1.0);
    glVertex3f(m_width,m_height,trf);
    QVector3D tr(m_width,m_height,trf);

//    glVertex3f(m_width,m_height,0.5);

    glColor4f(center_depth,brf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0, 0.0);
    glVertex3f(m_width,-m_height,brf);
    QVector3D br(m_width,-m_height,brf);
    glEnd();

    QVector3D n1 = QVector3D::crossProduct(br-bl,tl-bl);
//    qDebug()<<n1;
    _shaderParam.m_trueNormal = QVector3D(n1.x(),n1.y(), n1.z()).normalized();
//    _NormalControl->SetNormal3D(_shaderParam.m_trueNormal);
//    _shaderParam.m_trueNormal = _layerNormal;

//    if(n1.z()>0)
//        _shaderParam.m_trueNormal = n1.normalized();
//    else
//        _shaderParam.m_trueNormal = -n1.normalized();

//    qDebug()<<"bl"<<blf<<"tl"<<tlf<<"tr"<<trf<<"br"<<brf;
//    qDebug()<<"bl"<<bl<<"tl"<<tl<<"tr"<<tr<<"br"<<br;

    //    qDebug()<<"n1"<<n1;
//    qDebug()<<"n2"<<n2;

//    glVertex3f(m_width,-m_height,0.5);

//    glDepthMask(GL_TRUE);
    Session::get()->glWidget()->getMShader()->release();
    glDisable(GL_TEXTURE_2D);
    _shaderParam.m_centerDepth = QVector3D(m_width+P().x,-m_height+P().y,brf);
    _shaderParam.m_boundingbox[0] = -m_width+P().x;
    _shaderParam.m_boundingbox[1] = m_width+P().x;
    _shaderParam.m_boundingbox[2] = -m_height+P().y;
    _shaderParam.m_boundingbox[3] = m_height+P().y;

//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glBegin(GL_POINTS);
//    glColor3f(1.0,1.0,0.0);
//    glVertex3f(_shaderParam.m_centerDepth.x(),_shaderParam.m_centerDepth.y(),0.0);
//    glEnd();

}

void Light::render(int mode) {
    Selectable::render(mode);

    glColor3f(1.0, 1.0, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex3f(P().x, P().y, 0.001);
    glEnd();
}

void LayerNormalControl::render(int mode) {
    //Point p0 = _pShape->P();

    if(Session::isRender(NORMALS_ON)&&!(Session::isRender(PREVIEW_ON)||Session::isRender(AMBIENT_ON)||Session::isRender(SHADOWS_ON)))
    {
        Draggable::render(mode);
        glColor3f(1.0, 1.0, 0);
        glPointSize(8);
        glBegin(GL_POINTS);
        glVertex3f(P().x, P().y, 1.005);
        //    glVertex3f(EndPoint.x, EndPoint.y, 1.005);
        glEnd();
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 1.005);
        glVertex3f(P().x, P().y, 1.005);
        glEnd();
    }
}

/*
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

*/

#endif
