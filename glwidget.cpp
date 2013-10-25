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

#include "glwidget.h"

#include <QMouseEvent>
#include <QTimer>
#include <gl/glu.h>
#include <math.h>
#include "canvas.h"

GLWidget::GLWidget(QWidget *parent)
    : QGLWidget(parent)
{
    xRot = 0;
    _scale = 1.0;
    //setFocusPolicy(Qt::StrongFocus);
    /*QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(advance()));
    timer->start(20);*/
}

GLdouble MvMatrix[16];
GLdouble ProjMatrix[16];
GLint ViewPort[4];
GLuint SelectBuff[SELECT_BUFF_SIZE];

GLWidget::~GLWidget()
{
    //glDeleteLists(gear1, 1);
}

void GLWidget::setXRotation(int angle)
{
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void GLWidget::initializeGL()
{
     glShadeModel(GL_SMOOTH);
     //glDepthRange(0.0, 1.0);

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

     //Lighting
     GLfloat mat_specular[]   = { 1.0, 1.0, 1.0, 1.0 };
     GLfloat mat_shininess[]  = { 50.0 };
     GLfloat light_position[] = { -1.0, 1.0, 1.0, 0.0 };

     glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
     glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
     glLightfv(GL_LIGHT0, GL_POSITION, light_position);
     glEnable(GL_LIGHT0);

     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glTranslated(0, 0, -1.0);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    orthoView();
    glTranslated(0, 0, -1.0);

    glPushMatrix();
    renderCanvas();
    glPopMatrix();
}

void GLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
    _aspectR = width*1.0 / height;
    /*    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslated(_cam.x, _cam.y, -1.0);*/
}

void GLWidget::mousePressEvent(QMouseEvent *event)
{
    _lastP = event->pos();
    _lastWorldP = toWorld(_lastP.x(), _lastP.y());

    int hit = selectGL(_lastP.x(), _lastP.y());
    if (hit){
        Selectable_p pSel = select(hit, SelectBuff);
        if (Canvas::get()->canDragShape() && pSel->type() == Renderable::SHAPE){
            Canvas::get()->activate((Shape_p)pSel);
        }
        Selectable::startSelect(pSel, event->button() == Qt::LeftButton, event->modifiers() & Qt::ControlModifier);
    }

    //send the click to the active shape
    if (Canvas::get()->active() && !Canvas::get()->isDragMode){
        if (event->buttons()&Qt::LeftButton)
            Canvas::get()->active()->sendClick(_lastWorldP, Selectable::DOWN);
        else if (event->buttons()& Qt::RightButton)
             Canvas::get()->active()->sendClick(_lastWorldP, Selectable::R_DOWN);
    }

    updateGL();
}

void GLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Selectable::stopSelect();
    _lastP = event->pos();
    _lastWorldP = toWorld(_lastP.x(), _lastP.y());

    //send the click to the active shape
    if (Canvas::get()->active() && !Canvas::get()->isDragMode){
        if (event->button() == Qt::LeftButton )
            Canvas::get()->active()->sendClick(_lastWorldP, Selectable::UP);
        else if (event->button() == Qt::RightButton)
            Canvas::get()->active()->sendClick(_lastWorldP, Selectable::R_UP);
    }

    updateGL();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{   
    _lastP = event->pos();
    Point p = toWorld(_lastP.x(), _lastP.y());

    if ( event->modifiers() & Qt::AltModifier){
       _translate = _translate - (p - _lastWorldP);
    }else if (Canvas::get()->isDragMode){
        Draggable::dragTheSelected(p - _lastWorldP);
    }

    _lastWorldP = p;
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

}

void GLWidget::advance()
{
    updateGL();
}

void GLWidget::renderCanvas() const {

    Canvas::get()->render();
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

    glMatrixMode(GL_MODELVIEW);
    renderCanvas();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    int hits = glRenderMode(GL_RENDER);
    glMatrixMode(GL_MODELVIEW);
    return hits;
}

void GLWidget::orthoView(){
    glOrtho(-1.0*_scale+_translate.x, 1.0*_scale+_translate.x, -1.0/_aspectR*_scale+_translate.y, 1.0/_aspectR*_scale+_translate.y, NEAR_P, FAR_P);
}

Point GLWidget::toWorld(int x, int y){

    glGetIntegerv(GL_VIEWPORT, ViewPort);
    glGetDoublev(GL_MODELVIEW_MATRIX, MvMatrix);
    glGetDoublev(GL_PROJECTION_MATRIX, ProjMatrix);

    double winX = (double)x;
    double winY = (double)ViewPort[3] - (double)y;
    double winZ = 0;
    double wx, wy, wz;

    gluUnProject( winX, winY, winZ, MvMatrix, ProjMatrix, ViewPort, &wx, &wy, &wz);
    return Vec2(wx, wy);
}
