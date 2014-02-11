#include "glwidget.h"
#include "canvas.h"
#include "shapecontrol.h"
#include "Renderer/shaderprogram.h"
#include "Renderer/ywrenderer.h"
GLuint tt;

static bool          firstTime;
static unsigned char textureSt[4 * 640 * 640];
GLdouble             tM[16];

bool Session::isRender(RenderSetting rs){
    return GLWidget::is(rs);
}

void GLWidget::renderCanvas()
{
    ((YWRenderer*)Session::get()->renderer(1))->getRShader()->release();
    ((YWRenderer*)Session::get()->renderer(1))->getRShader()->SetInitialized(false);

    if (is(DRAGMODE_ON) && (is(SHADING_ON) || is(PREVIEW_ON)))
    {
        _pCanvas->_lights[0]->render();
    }

    FOR_ALL_CONST_ITEMS(ShapeList,_pCanvas->_shapes){ //need to add layers
        Shape_p s = *it;
        if (!s->isChild())
            render(*it);
    }
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

    pShape->render();

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
