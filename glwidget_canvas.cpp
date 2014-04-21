#include "glwidget.h"
#include "canvas.h"

#define UPDATE_GL Session::get()->glWidget()->updateGL();

GLuint texture[30];

GLuint bg_tex;
double bg_aspect;

void Canvas::insert(Shape_p sp)
{
    _shapes.push_front(sp);
    updateDepth();
    UPDATE_GL
}

void Canvas::remove(Shape_p sp)
{
    //m_GLSLShader->RemoveParamSet(sp->getShaderParam());
    _shapes.remove(sp);
    //delete sp;
    updateDepth();
}

Shape_p Canvas::findPrev(Shape_p pShape)
{
    if (!_shapes.size())
        return 0;

    if (_shapes.size() == 1)
        return _shapes.front();

    ShapeList::iterator it = std::find(_shapes.begin(), _shapes.end(), pShape);
    if (it != _shapes.end())
        it++;

    if (it == _shapes.end())
        it = _shapes.begin();
    return *it;
}

Shape_p Canvas::findNext(Shape_p pShape){
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

void Canvas::clear()
{
    FOR_ALL_ITEMS(ShapeList,_shapes)
        delete *it;
    _shapes.clear();
    updateDepth();
}

void Canvas::setImagePlane(const string &filename){
    if (!filename.empty())
        _pBGImage->readFromFile(filename);
   /* QImage img_data = QGLWidget::convertToGLFormat(QImage(QString::fromStdString(filename)));
    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &bg_tex);//;&texture[0]);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, img_data.width(), img_data.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data.bits() );
    bg_aspect =  (img_data.width()*1.0) / img_data.height();*/
}

ImagePlane::ImagePlane(){
    glGenTextures(1, &_tex);
}

bool ImagePlane::readFromFile(const string &filename){

    QImage img(QString::fromStdString(filename));

    if (img.isNull())
        return false;

    _img = QGLWidget::convertToGLFormat(img);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tex);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _img.width(), _img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, _img.bits() );
    _aspect = (_img.width()*1.0) / _img.height();
    return true;
}

bool  ImagePlane::hasTexture() const{ return !_img.isNull();}


void ImagePlane::render()
{
    glColor3f(0,0,0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, _tex);//bg_tex);

    // Draw a textured quad
    glColor4f(1, 1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex2f(-1.0*_aspect, -1.0);
    glTexCoord2f(0, 1); glVertex2f(-1.0*_aspect, 1.0);
    glTexCoord2f(1, 1); glVertex2f( 1.0*_aspect, 1.0);
    glTexCoord2f(1, 0); glVertex2f( 1.0*_aspect, -1.0);
    glEnd();

    /*glBegin(GL_QUADS);
    glTexCoord2f(0, 0); glVertex3f(0, 0, 0);
    glTexCoord2f(0, 1); glVertex3f(0, 100, 0);
    glTexCoord2f(1, 1); glVertex3f(100, 100, 0);
    glTexCoord2f(1, 0); glVertex3f(100, 0, 0);
    glEnd();*/

    glDisable(GL_TEXTURE_2D);
}

QRgb ImagePlane::getColor(const Point &p) const{
    double w = 2.0*_aspect;
    double h = 2.0;
    int x = (int)(_img.width()*(p.x + _aspect)/w);
    int y = (int)(_img.height()*(p.y + 1.0)/h);
    return  _img.pixel(x, y);
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
