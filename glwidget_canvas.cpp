#include "glwidget.h"
#include "canvas.h"

#define UPDATE_GL Session::get()->glWidget()->updateGL();

GLuint texture[30];

void Canvas::insert(Shape_p sp){
    _shapes.push_front(sp);
    updateDepth();
    UPDATE_GL
}

void Canvas::remove(Shape_p sp){
//  m_GLSLShader->RemoveParamSet(sp->getShaderParam());
    _shapes.remove(sp);
    //delete sp;
    updateDepth();
}

Shape_p Canvas::findPrev(Shape_p pShape){
    if (!_shapes.size())
        return 0;
    if (_shapes.size() == 1)
        return _shapes.front();

    ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), pShape);
    if (it == _shapes.begin())
        it = _shapes.end();
    it--;
    return *it;
}

Shape_p Canvas::findNext(Shape_p pShape){
    if (!_shapes.size())
        return 0;

    if (_shapes.size() == 1)
        return _shapes.front();

    ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), pShape);
    if (it != _shapes.end())
        it++;

    if (it == _shapes.end())
        it = _shapes.begin();
}

void Canvas::moveDown(Shape_p pShape){

    if (!pShape)
        return;

    ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), pShape);
    ShapeList::iterator itn = it;
    itn++;
    if ( itn == _shapes.end())
        return;
    Shape_p tmp = *it;
    (*it) = *itn;
    (*itn) = tmp;
    updateDepth();
}

void Canvas::moveUp(Shape_p pShape){

    if (!pShape)
        return;

    ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), pShape);
    ShapeList::iterator itp = it;
    if ( itp == _shapes.begin())
        return;
    itp--;
    Shape_p tmp = *it;
    (*it) = *itp;
    (*itp) = tmp;
    updateDepth();
}

void Canvas::sendToBack(Shape_p pShape){
    if (!pShape)
        return;
    _shapes.remove(pShape);
    _shapes.push_back(pShape);
    updateDepth();
}

void Canvas::sendToFront(Shape_p pShape){
    if (!pShape)
        return;
    _shapes.remove(pShape);
    _shapes.push_front(pShape);
    updateDepth();
}

void Canvas::clear(){
    FOR_ALL_ITEMS(ShapeList,_shapes)
        delete *it;
    _shapes.clear();
    updateDepth();
}

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


void Canvas::updateDepth()
{
#ifndef MODELING_MODE
    unsigned char m=0;
    FOR_ALL_ITEMS(ShapeList,_shapes)
    {
        (*it)->setLayerLabel(m++);
    }
#endif

}

//NOW GL WIDGET ////////////////////////////////////////////
