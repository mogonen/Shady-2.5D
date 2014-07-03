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
    _pSession->_pTheCommand     = 0;
    _pSession->_pTheShape       = 0;
    _pSession->_channel         = NORMAL_CHANNEL;

    _pSession->_pExporters[0]   = new INPExporter();
    _pSession->_pExporters[1]   = new EPSExporter();
}

void Session::activate(Shape_p sp){
    _pTheShape = sp;
    _pMainWindow->setAttrWidget((void*)sp);
    _pGlWidget->updateGL();
}

Shape* Session::deactivate(){
    _pMainWindow->setAttrWidget((void*)0);
    //if (_pTheShape) _pTheShape->onDeactivate();
    _pTheShape = 0;
    _pGlWidget->updateGL();
}

void Session::insertShape(Shape* pShape, bool isactivate){
    if (!pShape)
        return;
    _pCanvas->insert(pShape);
    _pMainWindow->insertShape(pShape);
    if (isactivate)
        activate(pShape);
    _pGlWidget->updateGL();
}

void Session::removeShape(Shape* pShape){
    if (!pShape)
        return;
    if (pShape == _pTheShape)
        deactivate();
    _pMainWindow->removeAttrWidget((void*)pShape);
    _pCanvas->remove(pShape);
    _pGlWidget->updateGL();
}

void Session::reset(){
    _pCanvas->clear();
    _filename = 0;
    if (_pTheCommand)
        _pTheCommand->cancel();
    _pGlWidget->updateGL();
}

void Session::open(const char* fname){
    if (_pFileIO->load(fname))
        _filename = fname;
}

void Session::saveAs(const char *fname){
    _filename = fname;
    _pFileIO->save(_filename);
}

void Session::exportShape(const char *fname, int exporterid){
    if (!_pTheShape)
        return;
    _filename = fname;
    _pExporters[exporterid]->exportShape(_pTheShape, _filename);
}


int Session::save(){
    if (!_filename)
        return -1;
    _pFileIO->save(_filename);
}

void  Session::setBG(const char *fname){
    _pCanvas->setImagePlane(fname);
    _pGlWidget->setRender(BACKGROUND_ON, true);
}

void  Session::setCommand(Command_p pCommand){
    if (_pTheCommand){
        _pTheCommand->unselect();
        delete _pTheCommand;
    }
    _pTheCommand = pCommand;
    if (_pTheCommand)
        _pTheCommand->select();
}

void  Session::exec(Command_p pCommand){

    if (!pCommand)
        pCommand = _pTheCommand;

    if (!pCommand)
        return;

    Command_p newcommand = pCommand->exec();
    _commands.push_back(pCommand);
    if (_pTheCommand)
        _pTheCommand = newcommand;

    _pSelectionMan->cancelSelection();
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

SelectionMode Session::selectMode(){
    return _pSession->_pTheCommand ? _pSession->_pTheCommand->selectMode() : NOSELECT;
}


Channel Session::channel(){
    return _pSession->_channel;
}

int   Session::redo(){
    //not for now
}

void Session::cancel(){
    if (!_pTheCommand)
        return
    _pTheCommand->cancel();
    _pGlWidget->updateGL();
}

void  Session::sendClick(const Click& clk){
    if (_pTheCommand)
        _pTheCommand->sendClick(clk);
    if (_pTheShape)
        _pTheShape->sendClick(clk);
}


void  Session::moveActiveDown(){
    _pCanvas->moveDown(_pTheShape);
    _pGlWidget->updateGL();
}

void  Session::moveActiveUp(){
    _pCanvas->moveUp(_pTheShape);
    _pGlWidget->updateGL();
}

void  Session::sendActiveBack(){
    _pCanvas->sendToBack(_pTheShape);
    _pGlWidget->updateGL();
}

void  Session::sendActiveFront(){
    _pCanvas->sendToFront(_pTheShape);
    _pGlWidget->updateGL();
}

void  Session::selectNext(){
    _pTheShape = _pCanvas->findNext(_pTheShape);
    _pGlWidget->updateGL();
}

void  Session::selectPrev(){
     _pTheShape = _pCanvas->findPrev(_pTheShape);
     _pGlWidget->updateGL();
}

void  Session::setChannel(Channel channel){
    _channel = channel;
    _pGlWidget->updateGL();
}

