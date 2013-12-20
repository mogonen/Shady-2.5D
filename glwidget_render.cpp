#include "glwidget.h"
#include "canvas.h"
#include "shapecontrol.h"
#include "Renderer/shaderprogram.h"
GLuint tt;

static bool          firstTime;
static unsigned char textureSt[4 * 640 * 640];
GLdouble             tM[16];

bool Session::isRender(RenderSetting rs){
    return GLWidget::is(rs);
}

void GLWidget::renderCanvas()
{
    _pGLSLShader_R->release();
    if(isInRenderMode() && (is(PREVIEW_ON)||is(AMBIENT_ON)||is(SHADOWS_ON)))
    {
        glReadBuffer(GL_BACK);
        if(!_pGLSLShader_R->isInitialized())
        {
            Session::get()->deactivate();
            _pGLSLShader_R->SetInitialized(true);
            FOR_ALL_CONST_ITEMS(ShapeList, _pCanvas->_shapes){ //need to add layers
                Shape_p pShape = *it;
                glPushMatrix();
                Point p = pShape->P();
                glTranslatef(p.x, p.y, 0);
                apply(pShape->getTransform());
                glMultMatrixd((double*)&tM);
                pShape->render(SM_MODE);
                glPopMatrix();
            }

            _pGLSLShader_R->GrabShapeMap(width(), height());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FOR_ALL_CONST_ITEMS(ShapeList, _pCanvas->_shapes){ //need to add layers
                Shape_p pShape = *it;
                glPushMatrix();
                Point p = pShape->P();
                glTranslatef(p.x, p.y, 0);
                apply(pShape->getTransform());
                glMultMatrixd((double*)&tM);
                pShape->type();
                pShape->render(BRIGHT_MODE);
                glPopMatrix();
            }
            _pGLSLShader_R->GrabBrightMap();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FOR_ALL_CONST_ITEMS(ShapeList, _pCanvas->_shapes){ //need to add layers
                Shape_p pShape = *it;
                glPushMatrix();
                Point p = pShape->P();
                glTranslatef(p.x, p.y, 0);
                apply(pShape->getTransform());
                glMultMatrixd((double*)&tM);
                pShape->type();
                pShape->render(DARK_MODE);
                glPopMatrix();
            }
            _pGLSLShader_R->GrabDarkMap();

            _pCanvas->updateDepth();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            FOR_ALL_CONST_ITEMS(ShapeList, _pCanvas->_shapes){ //need to add layers
                Shape_p pShape = *it;
                glPushMatrix();
                Point p = pShape->P();
                glTranslatef(p.x, p.y, 0);
                apply(pShape->getTransform());
                glMultMatrixd((double*)&tM);
                pShape->render(LABELDEPTH_MODE);
                glPopMatrix();
            }
            _pGLSLShader_R->GrabLDMap();

            _pGLSLShader_R->SetTextureToShader();
        }

        _pGLSLShader_R->bind();
        _pGLSLShader_R->LoadShaperParameters(_pCanvas->_shapes);
        _pGLSLShader_R->setUniformValue("width", (float)width());
        _pGLSLShader_R->setUniformValue("height", (float)height());
        _pGLSLShader_R->setUniformValue("toggle_ShaAmbCos", (int)(_renderFlags>>1)&7);
        float aspect = (float)height() / width();


        glBegin(GL_QUADS);
        glColor4f(1.0, 1.0, 1.0, 1.0);
        glTexCoord2d(0.0,0.0);
        glVertex2f(-1.0,-1.0);
        glTexCoord2d(0.0,1.0);
        glVertex2f(-1.0,1.0);
        glTexCoord2d(1.0,1.0);
        glVertex2f(1.0,1.0);
        glTexCoord2d(1.0,0.0);
        glVertex2f(1.0,-1.0);
        glEnd();

        _pGLSLShader_R->release();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);

        if (is(DRAGMODE_ON))
        {
            glColor3f(1.0, 1.0, 0);
            _pCanvas->_lights[0]->render(DRAG_MODE);
        }

    }
    else
    {
        _pGLSLShader_R->release();

        _pGLSLShader_R->SetInitialized(false);
        if (is(DRAGMODE_ON) && (is(SHADING_ON) || is(PREVIEW_ON)))
        {
            _pCanvas->_lights[0]->render(DRAG_MODE);
        }
        FOR_ALL_CONST_ITEMS(ShapeList,_pCanvas->_shapes){ //need to add layers
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


void GLWidget::render(Shape_p pShape)
{

    glPushMatrix();

    Point p = pShape->P();
    glTranslatef(p.x, p.y, 0);

    if (pShape == _pActiveShape)
        Session::get()->controller()->renderHandler();

    apply(pShape->getTransform());
    glMultMatrixd((double*)&tM);

    pShape->render(DRAG_MODE);

    if (pShape->isParent()){
        DraggableList childs = pShape->getChilds();
        FOR_ALL_CONST_ITEMS(DraggableList, childs){
            render((Shape_p)*it);
        }
    }
    glPopMatrix();
}

void GLWidget::apply(const Matrix3x3& M){
    tM[0] = M[0].x;
    tM[1] = M[1].x;
    tM[2] = 0;
    tM[3] = 0;

    tM[4] = M[0].y;
    tM[5] = M[1].y;
    tM[6] = 0;
    tM[7] = 0;

    tM[8]  = 0;
    tM[9]  = 0;
    tM[10] = 1;
    tM[11] = 0;

    tM[12] = M[0].z;
    tM[13] = M[1].z;
    tM[14] = 0;
    tM[15] = 1;
}
