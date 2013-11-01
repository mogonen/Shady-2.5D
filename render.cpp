#include <qgl.h>
#include <QDebug>
#include "Canvas.h"
#include "SampleShape.h"


#include "meshshape/spineshape.h"
#include "meshshape/meshshape.h"

#ifdef FACIAL_SHAPE
#include "FacialShape/facialshape.h"
#endif

#include "meshshape/Patch.h"
#include "controlpoint.h"
#include "curve.h"
#include "shapecontrol.h"
#include "ellipseshape.h"

//these inits need to be moved somewhere else
Canvas* Canvas::_canvas = new Canvas();
int Selectable::_COUNT = 0;
Selectable_p Selectable::_theSelected = 0;
Selectable_p Selectable::_lastSelected = 0;
SelectableMap_p Selectable::_selectables;
SelectionSet Selectable::_selection;
bool Selectable::isSelect;
ControlPoint_p ControlPoint::_pTheActive = 0;
Canvas::EditMode_e Canvas::MODE = Canvas::POINT_NORMAL_SHAPE_M;
double Canvas::_tM[16];
double EllipseShape::Radius = 0.1;
GLuint texture[30];
GLuint tt;

static bool firstTime;

static unsigned char textureSt[4 * 640 * 640];


//selection stuff
bool isInRenderMode(){
    GLint mode;
    glGetIntegerv(GL_RENDER_MODE, &mode);
    return ( mode == GL_RENDER);
}

bool MeshShape::IsSelectMode(SELECTION_e eMode){
    return GetSelectMode() == eMode && !Canvas::get()->isDragMode;
}

void Selectable::renderNamed(bool ispush) const{
    glLoadName(name());
    render();
}

Selectable_p select(GLint hits, GLuint *buff){
   //only one selectable object in the stack for now
   unsigned int name = buff[3];
   Selectable_p pSel =0;

   if ( name & ( 1 << UI_BIT ) ){
      pSel = ShapeControl::get();
      ShapeControl::get()->startSelect(name);
   }else
      pSel = Selectable::get(buff[3]);

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
void Canvas::setImagePlane(const string &filename){
    QImage img_data = QGLWidget::convertToGLFormat(QImage(QString::fromStdString(filename)));
    glGenTextures(30, texture);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  img_data.width(), img_data.height(),
                  0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.bits() );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}

void Canvas::render() const{

    if(Canvas::get()->isShadingOn||Canvas::get()->isAmbientOn||Canvas::get()->isShadowOn)
    {
        glReadBuffer(GL_BACK);
        if(!Canvas::get()->m_GLSLShader->isInitialized())
        {
            Canvas::get()->deactivate();
            Canvas::get()->m_GLSLShader->SetInitialized(true);
            Canvas::get()->m_GLSLShader->setUniformValue("width", (float)Canvas::get()->width);
            Canvas::get()->m_GLSLShader->setUniformValue("height", (float)Canvas::get()->height);
            FOR_ALL_CONST_ITEMS(ShapeList, _shapes){ //need to add layers
                Shape_p pShape = *it;
                glPushMatrix();
                Point p = pShape->P();
                glTranslatef(p.x, p.y, 0);
                apply(pShape->getTransform());
                glMultMatrixd((double*)&_tM);
                pShape->renderAll();
                glPopMatrix();
            }
            Shape_p pShape = _shapes.front();
            ShaderParameters *sp;
            sp = pShape->getShaderParam();
            if(!sp)
            {
                sp = pShape->initializeParam();
                sp->AssociateToProgram(Canvas::get()->m_GLSLShader);
            }
            pShape->setBrightParam();
            pShape->setDarkParam();
            sp->GrabShapeMap(Canvas::get()->width,Canvas::get()->height);
            sp->SetAllLocalParameters();
        }
        m_GLSLShader->bind();
        float aspect = (float)Canvas::get()->height/Canvas::get()->width;

        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2d(0.0,0.0);
        glVertex3f(-1.0,-1.0,0);
        glTexCoord2d(0.0,1.0);
        glVertex3f(-1.0,1.0,0);
        glTexCoord2d(1.0,1.0);
        glVertex3f(1.0,1.0,0);
        glTexCoord2d(1.0,0.0);
        glVertex3f(1.0,-1.0,0);
        glEnd();

        m_GLSLShader->release();

        glScalef(0.99,0.99,0.9);

        glBegin(GL_LINE_LOOP);
        glVertex3f(-1.0,-1.0*aspect,0);
        glVertex3f(-1.0,1.0*aspect,0);
        glVertex3f(1.0,1.0*aspect,0);
        glVertex3f(1.0,-1.0*aspect,0);
        glEnd();
        glDisable(GL_TEXTURE_2D);
    }
    else
    {
        m_GLSLShader->release();


        if (Canvas::get()->isShadingOn && Canvas::get()->isDragMode)
        {
            _lights[0]->renderNamed();
        }
        FOR_ALL_CONST_ITEMS(ShapeList, _shapes){ //need to add layers
            Shape_p s = *it;
            if (!s->isChild())
                render(*it);
        }
    }
//    glBindTexture(GL_TEXTURE_2D, tt);

////    if (!firstTime)
//    {
//        glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,0,0,1024,1024,0); // copy whole image to textureimage
//        firstTime = true;
//        qDebug()<<"123";
//        qDebug()<<"222";

//    }
////    else
//    {
//        glReadBuffer(GL_BACK);
//        glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,50,50,0,640,640);
//        qDebug()<<glGetError();
//    }

//    glDisable(GL_TEXTURE_2D);

////    glTexImage2D(GL_TEXTURE_2D, 0 ,4, 640, 640, 0, GL_RGBA8, GL_UNSIGNED_BYTE, textureSt);
////    QImage tempimg((const unsigned char*)textureSt, 640, 640, QImage::Format_ARGB32);
////    tempimg.save("test.png");

//    if(tt)
//    {
//        glPushMatrix();
//        glTranslatef(0.5,0.5,0.0);
//        glScalef(0.25,0.25,0.25);



//        glColor3f(1.0,1.0,0.0);
//        glBegin(GL_LINE_LOOP);
//        glVertex3f(-1.0,-1.0,0);
//        glVertex3f(-1.0,1.0,0);
//        glVertex3f(1.0,1.0,0);
//        glVertex3f(1.0,-1.0,0);
//        glEnd();
//        glDisable(GL_TEXTURE_2D);
//        glPopMatrix();

//    }
}

void Light::render() const {
    glColor3f(1.0, 1.0, 0);
    glPointSize(8);
    glBegin(GL_POINTS);
    glVertex3f(P().x, P().y, 0);
    glEnd();

}

void Canvas::render(Shape_p pShape) const {

    glPushMatrix();

    Point p = pShape->P();
    glTranslatef(p.x, p.y, 0);

    if (pShape == Canvas::get()->active())
        ShapeControl::get()->renderHandler();

    apply(pShape->getTransform());
    glMultMatrixd((double*)&_tM);

    pShape->renderAll();

    if (pShape->isParent()){
        DraggableList childs = pShape->getChilds();
        FOR_ALL_CONST_ITEMS(DraggableList, childs){
            render((Shape_p)*it);
        }
    }
    glPopMatrix();
}

void SampleShape::render() const{

    glColor3f(1.0, 0, 0);
    glBegin(GL_POLYGON);
    /*for(int i = 0; i < 4; i++)
        glVertex3f(p[i].x, _p[i].y, 0);*/
    glEnd();
}

void ControlPoint::render() const {

    if (isChild() && !isActive()) return;

    glColor3f(1.0, 1.0, 1.0);
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

void Shape::renderAll() const
{
    if(Canvas::get()->isDragMode && this == Canvas::get()->active())
        ShapeControl::get()->renderControls((Shape_p)this);
    renderNamed(true);
}
void ShapeControl::renderControls(Shape_p shape) const{
    if (_theHandler->isActive())
        return;
    //could not get pushname popname working!
    glPointSize(4.0);
    SVList verts = shape->getVertices();
    FOR_ALL_CONST_ITEMS(SVList, verts){
        ShapeVertex_p sv = (*it);
        if (sv->parent() && (!_theSelected || (_theSelected != sv->parent() && _theSelected->parent() != sv->parent())) )
            continue;

        unsigned int svname = sv->id() | (1 << UI_BIT);
        if (Canvas::get()->isNormalsOn && sv->isNormalControl){
            unsigned int svn_name = svname | (1 << NORMAL_CONTROL_BIT);
            Point p1 = sv->P + Point(sv->N*NORMAL_RAD);
            glLoadName(svn_name);
            glBegin(GL_POINTS);
            glColor3f(1.0, 0, 0);
            glVertex3f(p1.x, p1.y, 0);
            glEnd();
            if (isInRenderMode()){
                glColor3f(1.0, 1.0, 1.0);
                glBegin(GL_LINES);
                glVertex3f(sv->P.x, sv->P.y, 0);
                glVertex3f(p1.x, p1.y, 0);
                glEnd();
            }
        }

        glLoadName(svname);
        glBegin(GL_POINTS);
        glColor3f(1.0, 1.0, 1.0);
        glVertex3f(sv->P.x, sv->P.y, 0);
        glEnd();

        if (isInRenderMode() && sv->parent()){
            glBegin(GL_LINES);
            glVertex3f(sv->P.x, sv->P.y, 0);
            glVertex3f(sv->parent()->P.x, sv->parent()->P.y, 0);
            glEnd();
        }
    }
}

void SpineShape::render() const{

    glBegin(GL_LINES);
        FOR_ALL_CONST_ITEMS(SVertexList, _verts){
            if (!(*it)->pP)
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

void MeshShape::render() const {

    if ( Canvas::MODE!=Canvas::SHADED_M && (isInRenderMode() || IsSelectMode(EDGE) )){
        EdgeList edges = _control->edges();
        FOR_ALL_CONST_ITEMS(EdgeList, edges){
            render(*it);
        }
    }

    //too messy, fix it!
    if (isInRenderMode() || (IsSelectMode(FACE) || Canvas::get()->canDragShape() ) ){
        FaceList faces = _control->faces();
        FOR_ALL_CONST_ITEMS(FaceList, faces){
            render(*it);
        }
    }
}

void MeshShape::render(Edge_p pEdge) const{
    if (this != Canvas::get()->active())
        return;
    if (pEdge->pData->pCurve){
        pEdge->pData->pCurve->renderNamed();
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

void MeshShape::render(Face_p pFace) const{
    if (pFace->pData->pSurface){
        //this is messy, needs better solution
        if (Canvas::get()->canDragShape()){
            pFace->pData->pSurface->renderUnnamed();
        }else{
            pFace->pData->pSurface->renderNamed();
        }
        return;
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

void Curve::render() const {

    glColor3f(1.0, 1.0, 1.0);
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


void Patch4::render() const{

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

            if (isInSelection())
            {
                selectionColor((Selectable_p)this);
                glBegin(GL_POLYGON);
                for(int k=0; k<4; k++){
                    glVertex3f(p[k].x, p[k].y, 0);
                }
                glEnd();
                continue;
            }



            if (Canvas::get()->isWireframeOn){

                if (Canvas::MODE == Canvas::NORMAL_M || Canvas::get()->isNormalsOn)
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

//            if (Canvas::MODE == Canvas::SHADED_M || Canvas::get()->isShadingOn ){
//                glEnable(GL_LIGHTING);
//                Point light0_p = Canvas::get()->lightPos(0);
//                GLfloat light0_pf[] = { light0_p.x, light0_p.y, 5.0, 0.0 };
//                glLightfv(GL_LIGHT0, GL_POSITION, light0_pf);
//            }else
//                glDisable(GL_LIGHTING);

            glBegin(GL_POLYGON);
            for(int k=0; k<4; k++){                
//                if (Canvas::MODE == Canvas::SHADED_M || Canvas::get()->isShadingOn)
//                    glNormal3f(n[k].x, n[k].y, n[k].z );
//                else
                    glColor3f((n[k].x+1)/2, (n[k].y+1)/2, n[k].z );
               glVertex3f(p[k].x, p[k].y, 0);
               glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();

            glDisable(GL_LIGHTING);

        }
    }
}


void EllipseShape::render() const {

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

            if (Canvas::get()->isWireframeOn){

                if (Canvas::MODE == Canvas::NORMAL_M || Canvas::get()->isNormalsOn)
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

//            if (Canvas::get()->isShadingOn ){
//                glEnable(GL_LIGHTING);
//                Point light0_p = Canvas::get()->lightPos(0);
//                GLfloat light0_pf[] = { light0_p.x, light0_p.y, 5.0, 0.0 };
//                glLightfv(GL_LIGHT0, GL_POSITION, light0_pf);
//            }else
//                glDisable(GL_LIGHTING);


            glBegin(GL_POLYGON);
            for(int k = 0; k < size; k++){
//                if (Canvas::get()->isShadingOn)
//                    glNormal3f(n[k].x, n[k].y, n[k].z );
//                else
                    glColor3f((n[k].x+1)/2, (n[k].y+1)/2, n[k].z );
                glVertex2f(p[k].x, p[k].y);
                glTexCoord2d((p[k].x+1)/2, (p[k].y+1)/2);
            }
            glEnd();

            glDisable(GL_LIGHTING);
        }
    }
}

void TransformHandler::render() const{

    if (!_pShape || _pShape!=Canvas::get()->active())
        return;

    _handles[0]->renderNamed();
    _handles[1]->renderNamed();
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

