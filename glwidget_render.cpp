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
#ifdef MODELING_MODE
void GLWidget::renderCanvas()
{
  renderShapes();
}
#else
void GLWidget::renderCanvas()
{
    _pGLSLShader_R->release();
    if(isInRenderMode() && is(PREVIEW_ON))
    {
        glReadBuffer(GL_BACK);
        if(!_pGLSLShader_R->isInitialized())
        {
            Session::get()->deactivate();
            _pGLSLShader_R->SetInitialized(true);
            renderShapes(SM_MODE);

            _pGLSLShader_R->GrabShapeMap(width(), height());

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderShapes(BRIGHT_MODE);
            _pGLSLShader_R->GrabBrightMap();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderShapes(DARK_MODE);
            _pGLSLShader_R->GrabDarkMap();

            _pCanvas->updateDepth();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderShapes(LABELDEPTH_MODE);
            _pGLSLShader_R->GrabLDMap();

            _pGLSLShader_R->SetTextureToShader();
        }

        _pGLSLShader_R->bind();
        _pGLSLShader_R->LoadShaperParameters(_pCanvas->_shapes);
        _pGLSLShader_R->setUniformValue("width", (float)width());
        _pGLSLShader_R->setUniformValue("height", (float)height());
        _pGLSLShader_R->setUniformValue("toggle_ShaAmbCos", (int)(_renderFlags>>1)&7);

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

        if (is(DRAG_ON))
        {
            glColor3f(1.0, 1.0, 0);
            _pCanvas->_lights[0]->render();
        }

        glColor3f(1.0, 1.0, 0);
        QFont m_font;
        m_font.setPointSize(15);
        QVector3D mousePos = _pGLSLShader_R->m_MousePos;
        QString z_pos = QString("pos: %1 %2 %3").arg(mousePos.x(),0,'f',3).arg(mousePos.y(),0,'f',3).arg(mousePos.z(),0,'f',3);
        renderText(10, 20, z_pos, m_font);
    }
    else
    {
        _pGLSLShader_R->release();
        _pGLSLShader_R->SetInitialized(false);
        if (is(DRAG_ON) && (is(SHADING_ON) || is(PREVIEW_ON)))
        {
            _pCanvas->_lights[0]->render();
        }

        if (is(SHADING_ON))
        {
            glEnable(GL_LIGHTING);
            Point light0_p      = Session::get()->canvas()->lightPos(0);
            GLfloat light0_pf[] = {light0_p.x, light0_p.y, 2, 1.0 };
            glLightfv(GL_LIGHT0, GL_POSITION, light0_pf);
        }

        renderShapes();
        glDisable(GL_LIGHTING);
    }

}
#endif

void GLWidget::renderShapes(int mode){
    FOR_ALL_CONST_ITEMS(ShapeList,_pCanvas->_shapes){ //need to add layers
        Shape_p s = *it;
        if (!s->isChild())
            render(*it, mode);
    }
}


void GLWidget::render(Shape_p pShape, int mode)
{

    glPushMatrix();

    Point p = pShape->P();
    glTranslatef(p.x, p.y, 0);

    if (pShape == Session::get()->theShape() && mode==DEFAULT_MODE)
        Session::get()->controller()->renderHandler();

    apply(pShape->getTransform());
    glMultMatrixd((double*)&tM);
\
    pShape->Shape::render(mode);
    pShape->render(mode);

    if (pShape->isParent()){
        DraggableList childs = pShape->getChilds();
        FOR_ALL_CONST_ITEMS(DraggableList, childs){
            render((Shape_p)*it, mode);
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
