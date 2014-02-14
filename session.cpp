#include "base.h"
#include "fileio.h"
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
    _pSession->_pFileIO         = new DefaultIO();
    _pSession->_filename        = 0;
    _pSession->_pCommand        = 0;
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
    _filename = 0;
    if (_pCommand)
        _pCommand->cancel();
}

void Session::open(const char* fname){
    if (_pFileIO->load(fname))
        _filename = fname;
}

void Session::saveAs(const char *fname){
    _filename = fname;
    _pFileIO->save(_filename);
}

int Session::save(){
    if (!_filename)
        return -1;
    _pFileIO->save(_filename);
}

void  Session::setCommand(Command_p pCommand){
    if (_pCommand){
        _pCommand->unselect();
        delete _pCommand;
    }
    _pCommand = pCommand;
    _pCommand->select();
}

void  Session::exec(){
    if (!_pCommand)
        return;
    Command_p newcommand = _pCommand->exec();
    _commands.push_back(_pCommand);
    _pCommand = newcommand;
    _pGlWidget->updateGL();
}

int   Session::undo(){
    if (_commands.empty())
        return 0;
    Command_p pCommand = _commands.back();
    _commands.pop_back();
    pCommand->unexec();
    _pGlWidget->updateGL();
    return _commands.size()+1;
}

int   Session::redo(){
    //not for now
}

void Session::cancel(){
    if (!_pCommand)
        return
    _pCommand->cancel();
    _pGlWidget->updateGL();
}

