#include "base.h"
#include "mainwindow.h"
#include "shapecontrol.h"
#include "canvas.h"
#include "glwidget.h"

Session* Session::_pSession = 0;

GLWidget* MainWindow::glWidget = 0;

void MainWindow::updateGL(){
    if (glWidget)
        glWidget->updateGL();
}

void Session::init(MainWindow * pMW){
    _pSession = new Session();
    _pSession->_pMainWindow     = pMW;
    _pSession->_pSelectionMan   = new SelectionManager();
    _pSession->_pController     = new ShapeControl();
    _pSession->_pCanvas         = new Canvas();
    _pSession->_pGlWidget       = new GLWidget(_pSession->_pCanvas);
}

void Session::activate(Shape_p sp){
    _pGlWidget->activate(sp);
    _pMainWindow->setAttrWidget((void*)sp);
}

Shape* Session::deactivate(){
    _pMainWindow->setAttrWidget((void*)0);
    return _pGlWidget->deactivate();
}

Shape* Session::theShape() const{
    return _pGlWidget->activeShape();
}

void Session::insertShape(Shape* pShape, bool isactivate){
    if (!pShape)
        return;
    _pCanvas->insert(pShape);
    if (isactivate)
        activate(pShape);
    _pGlWidget->updateGL();
}

void Session::removeShape(Shape* pShape){
    if (!pShape)
        return;
    if (pShape == _pGlWidget->activeShape())
        _pGlWidget->deactivate();
    _pMainWindow->removeAttrWidget((void*)pShape);
    _pCanvas->remove(pShape);
    _pGlWidget->updateGL();
}

void Session::reset(){
    _pCanvas->clear();
    _pGlWidget->clear();
}

/*void Session::cancel(){
    _pController->cancel();
    _pGlWidget->deactivate();
}

void Session::cancel(){
    _pController->cancel();
    _pGlWidget->deactivate();
}*/
