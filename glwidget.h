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

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include "base.h"
#include "Renderer/shaderprogram.h"

#define NEAR_P -100.0
#define FAR_P 100.0
#define MAX_SELECT 16
#define SELECT_BUFF_SIZE (MAX_SELECT*4)
#define PICK 8.0

class SelectionManager;
class Canvas;
class Shape;
class ShaderProgram;

Selectable_p select(GLint hits, GLuint* buff);

class GLWidget : public QGLWidget, public QGLFunctions
{
    Q_OBJECT

public:
    GLWidget(Canvas*, QWidget *parent = 0);

    static Point        toWorld(int, int);
    static Vec3         toCamera(float wx, float wy, float wz);
    static void         loadCameraParameters();

    void                apply(const Matrix3x3& tM);
\
    static bool         is(RenderSetting rs){return _renderFlags&(1<<rs);}
    void                setRender(RenderSetting rs, bool);
    void                flipDragMode();

    void                clear();
    void                keyPressEvent(QKeyEvent *);

protected:
    void                initializeGL();
    void                paintGL();
    void                resizeGL(int width, int height);
    void                mousePressEvent(QMouseEvent *event);
    void                mouseReleaseEvent(QMouseEvent *event);
    void                mouseMoveEvent(QMouseEvent *event);
    void                wheelEvent(QWheelEvent* e );
    void                setView();

private:

    void                renderCanvas();
    void                preview();

    void                render(Shape*, int mode=0);
    void                renderShapes(int mode=0);

    int                 selectGL(int, int);
    void                orthoView();

    double              _scale;
    double              _aspectR;
//    Vec2                _translate;

    QPoint              _lastP;
    Point               _lastWorldP;
    Vec2                _cam;

    //Shape               *_pActiveShape;
    Canvas              *_pCanvas;

    static unsigned int  _renderFlags;

    int                 _width0;

 public slots:
    void updateActive();

#ifndef MODELING_MODE
//************************ NOW PREVIEW STUFF ***********************************
public:
    Shape*              removeActive();
    ShaderProgram*      getRShader(){return _pGLSLShader_R;}
    ShaderProgram*      getMShader(){return _pGLSLShader_M;}
    ShaderProgram*      getCShader(){return _pGLSLShader_C;}

    void                reloadShader(){_pGLSLShader_R->ReloadShader();_pGLSLShader_M->ReloadShader();}
    void                updateGLSLLight(float x, float y, float z);

private:
    ShaderProgram       *_pGLSLShader_R;
    ShaderProgram       *_pGLSLShader_M;
    ShaderProgram       *_pGLSLShader_C;

    QVector2D           _mousePressPosition;
    QVector3D           _rotationAxis;
    qreal               _angularChange;
    QQuaternion         _rotation;
    qreal               _translateX;
    qreal               _translateY;
    qreal               _translateZ;

#endif
};

#endif // GLWIDGET_H
