/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include <gl/glu.h>
//#include <math.h>
#include "glwidget.h"
#include <QMouseEvent>
#include <QTimer>

#include "canvas.h"
//#include "Renderer/shaderprogram.h"

unsigned int    GLWidget::_renderFlags;

GLuint          SelectBuff[SELECT_BUFF_SIZE];
GLdouble        MvMatrix[16];
GLdouble        ProjMatrix[16];
GLint           ViewPort[4];

bool            m_CameraChanged;

/// @brief The Origin(0,0,0) screen depth position under current camera coordinates
double          m_ZeroX,m_ZeroY,m_ZeroZ;

GLWidget::GLWidget(Canvas * pCanvas, QWidget *parent)
    : QGLWidget(parent)
{
    _scale = 1.0;
    _renderFlags = 0x00;
    _pCanvas = pCanvas;

    _translateX = 0;
    _translateY = 0;
    _translateZ = 0;

    setRender(DRAGMODE_ON, true);
}

void GLWidget::setRender(RenderSetting rs, bool set){
    if (set)
        _renderFlags |= (1 << (int)rs);
    else
        _renderFlags &=~(1 << (int)rs);

    updateGL();
}

void GLWidget::flipDragMode(){
    //_renderFlags = _renderFlags | (_renderFlags^( 1 << (int)DRAGMODE_ON));
}

void GLWidget::initializeGL()
{
     glShadeModel(GL_SMOOTH);
     //glDepthRange(0.0, 1.0);

     glClearColor(0.0,0.0,0.0,0.0);

     glEnable(GL_DEPTH_TEST);
     glEnable(GL_NORMALIZE);

     glEnable( GL_ALPHA_TEST );
     glEnable(GL_TEXTURE_2D);

     glEnable(GL_BLEND);
     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

     glEnable(GL_TEXTURE_2D);
     glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

     glClear (GL_COLOR_BUFFER_BIT);
     _cam.set(0,0);

     //glClearColor(1.0, 1.0, 1.0, 1.0);
     //Lighting
     GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
     GLfloat mat_shininess[]  = { 50.0 };
     GLfloat light_position[] = { -1.0, 1.0, 1.0, 0.0 };


     float colour[4] = {0,0,0,0};
     glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,colour) ;
     glLightfv(GL_LIGHT0,GL_SPECULAR,colour);

//     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
//     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glEnable(GL_LIGHT0);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glTranslatef(0, 0, -2.0);


     m_CameraChanged = true;

     //meeds a better way
     Session::get()->renderer(1)->init();

}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    orthoView();

    setView();

    glPushMatrix();
    renderCanvas();
    glPopMatrix();
}

void GLWidget::setView()
{
    QMatrix4x4 matrix;
    matrix.translate(_translateX, _translateY, _translateZ);
    matrix.rotate(_rotation);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glMultMatrixf(matrix.constData());
}


void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    _aspectR = width*1.0 / height;
    m_CameraChanged = true;
    /*
     *glMatrixMode(GL_MODELVIEW);
     *glLoadIdentity();
    glTranslated(_cam.x, _cam.y, -1.0);*/
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    _lastP = event->pos();
    _lastWorldP = toWorld(_lastP.x(), _lastP.y());

    //send the click to the active shape
    int hit = selectGL(_lastP.x(), _lastP.y());

    //render mode only allows selection of light
    if(isInRenderMode() && (is(PREVIEW_ON)||is(AMBIENT_ON)||is(SHADOWS_ON)))
    {
        if (hit){
            Selectable_p pSel = select(hit, SelectBuff);
            if (is(DRAGMODE_ON) && pSel->type() == Renderable::UI)
            {
                Session::get()->selectionMan()->startSelect(pSel, event->button() == Qt::LeftButton, event->modifiers() & Qt::ControlModifier);
            }
        }
    }
    else
    {
        if (hit){
            Selectable_p pSel = select(hit, SelectBuff);
            if (is(DRAGMODE_ON) && pSel->type() == Renderable::SHAPE){
                Session::get()->activate((Shape_p)pSel);
            }
            Session::get()->selectionMan()->startSelect(pSel, event->button() == Qt::LeftButton, event->modifiers() & Qt::ControlModifier);
        }

        if (_pActiveShape && !is(DRAGMODE_ON)){
            if (event->buttons()&Qt::LeftButton)
                _pActiveShape->sendClick(_lastWorldP, Selectable::DOWN);
            else if (event->buttons()& Qt::RightButton)
                _pActiveShape->sendClick(_lastWorldP, Selectable::R_DOWN);
        }
        updateGL();
    }
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //render mode does not need release
    if(!(isInRenderMode() && (is(PREVIEW_ON)||is(AMBIENT_ON)||is(SHADOWS_ON))))
    {

        Session::get()->selectionMan()->stopSelect();
        _lastP = event->pos();
        _lastWorldP = toWorld(_lastP.x(), _lastP.y());

        //send the click to the active shape
        if (_pActiveShape && !is(DRAGMODE_ON)){
            if (event->button() == Qt::LeftButton )
               _pActiveShape->sendClick(_lastWorldP, Selectable::UP);
            else if (event->button() == Qt::RightButton)
               _pActiveShape->sendClick(_lastWorldP, Selectable::R_UP);
        }

        updateGL();
    }
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    QVector2D diff = QVector2D(event->pos()-_lastP);
    _lastP  = event->pos();
    Point p = toWorld(_lastP.x(), _lastP.y());

    if(event->modifiers() & Qt::ShiftModifier)
    {
        if((event->buttons() & Qt::RightButton)) {
            _translateX += diff.x()/100;
            _translateY -= diff.y()/100;
        }
        else
        {
        QVector3D n = QVector3D(diff.y(), diff.x(), 0.0).normalized();
        qreal acc = diff.length() / 4.0;
        _rotationAxis = (_rotationAxis * _angularChange + n * acc).normalized();
        _angularChange = acc;
        _rotation = QQuaternion::fromAxisAndAngle(_rotationAxis, _angularChange) * _rotation;
        }
    }
    else
    {
        if(event->buttons() & Qt::LeftButton)
        {
            if (is(DRAGMODE_ON))
                Session::get()->selectionMan()->dragTheSelected(p - _lastWorldP);
        }
        else
        {
            if (is(DRAGMODE_ON))
                Session::get()->selectionMan()->dragTheSelected(p - _lastWorldP, 1);
        }
    }
    _lastWorldP = p;
    m_CameraChanged = true;
    updateGL();
}

void GLWidget::wheelEvent(QWheelEvent* e ){

    if ( e->modifiers() & Qt::AltModifier){
        _scale-=(e->delta()/120.0 * 0.1);
        _scale = CLAMP(_scale, 0.1, 8.0);
        updateGL();
    }
}

void GLWidget::keyPressEvent(QKeyEvent * event){

    switch(event->key())
    {
    case Qt::Key_Enter:
        _translateX = 0;
        _translateY = 0;
        _translateZ = 0;
        _rotationAxis = QVector3D(0.0,0.0,0.0);
        _rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0,0.0,1.0),0.0);
    }
    updateGL();
}

int GLWidget::selectGL(int x, int y){

    glSelectBuffer(SELECT_BUFF_SIZE, SelectBuff);
    glGetIntegerv(GL_VIEWPORT, ViewPort);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    gluPickMatrix(x, ViewPort[3]-y, PICK, PICK, ViewPort);
    orthoView();
    setView();

//    glTranslatef(0.0,0.0,-1.0);
//    glMatrixMode(GL_MODELVIEW);

    renderCanvas();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    int hits = glRenderMode(GL_RENDER);
    glMatrixMode(GL_MODELVIEW);
    return hits;
}

void GLWidget::orthoView(){
//    glOrtho(-1.0*_scale+_translate.x, 1.0*_scale+_translate.x, -1.0/_aspectR*_scale+_translate.y, 1.0/_aspectR*_scale+_translate.y, NEAR_P, FAR_P);
    glOrtho(-1.0*_scale, 1.0*_scale, -1.0/_aspectR*_scale, 1.0/_aspectR*_scale, NEAR_P, FAR_P);
}

Point GLWidget::toWorld(int x, int y){

    if(m_CameraChanged)
        loadCameraParameters();
    double winX = (double)x;
    double winY = (double)ViewPort[3] - (double)y;
    double winZ = m_ZeroZ;
    double wx, wy, wz;

    gluUnProject( winX, winY, winZ, MvMatrix, ProjMatrix, ViewPort, &wx, &wy, &wz);
    return Vec2(wx, wy);
}

Vec3 GLWidget::toCamera(float wx, float wy, float wz)
{
    if(m_CameraChanged)
        loadCameraParameters();
    double cx,cy,cz;
    gluProject(wx,wy,wz,MvMatrix,ProjMatrix,ViewPort,&cx,&cy,&cz);
    return Vec3(cx,cy,cz);
}

void GLWidget::loadCameraParameters()
{
    //Load openGL ModelView/Project/Viewpot parameters
    glGetIntegerv(GL_VIEWPORT, ViewPort);           // Retrieves The Viewport Values (X, Y, Width, Height)
    glGetDoublev(GL_MODELVIEW_MATRIX, MvMatrix);       // Retrieve The Modelview Matrix
    glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);     // Retrieve The Projection Matrix

//    //Not know why, but it solves the problem, maybe some issue with QT
//    if(this->width()<this->height())
//        m_GLviewport[1] = -m_GLviewport[1];

    //After this function is called, set the CameraChanged to be false to prevent recall of this function
    m_CameraChanged = false;
    //calculate the depth value of the origin under current camera setting
    gluProject(0,0,0, MvMatrix,ProjMatrix,ViewPort,&m_ZeroX,&m_ZeroY,&m_ZeroZ);
}


