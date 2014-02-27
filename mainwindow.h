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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <QMainWindow>
#include <QtWidgets>
#include "customdialog.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QMenu;
class QScrollArea;
class QSlider;
QT_END_NAMESPACE

class GLWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void  keyPressEvent(QKeyEvent *event);

    int  addOptionsWidget(QWidget* widget, int key);
    void setOptionsWidget(int);

    int  addAttrWidget(QWidget* widget, void* key);
    void removeAttrWidget(void*);
    void setAttrWidget(void *);

    static void updateGL();
    static GLWidget        *glWidget;

private slots:

    void newFile();
    void openFile();
    void saveFile();
    void saveFileAs();

    void undo();
    void redo();
    void about();

    void viewOptions();
    void viewAttr();

    void flipDrag();
    void flipRender();
    void unselectDrag();

    void selectExtrudeEdge();
    void selectExtrudeFace();
    void selectInsertSegment();
    void selectDeleteFace();

    void toggleNormals();
    void togglePathces();
    void toggleShading();
    void toggleAmbient();
    void toggleShadow();
    void togglePreview();
    //void toggleShapeMap();


    void newGrid();
    void new2NGon();
    void newTorus();
    void newSpine();
    void newFacial();
    void newImageShape();
    void insertEllipse();

    void toggleLockShape();
    void parentShape();
    void groupShape();
    void transformShape();
    void moveShapeToBack();
    void moveShapeToFront();
    void sendShapeBack();
    void sendShapeFront();
    void deleteShape();

    void selectAssignPatternTool();
    void selectSetFoldsTool();


private:


    CustomDialog*   _options;

    void createActions();
    void createMenus();
    void initScene();
    void initTools();
	
	// for dockable widgets window
    void             createAllOptionsWidgets();

    QStackedWidget  *optionsStackedWidget;
    QStackedWidget  *attrStackedWidget;
    QDockWidget     *optionsDockWidget;
    QDockWidget     *attrDockWidget;
    QDockWidget     *rendererDockWidget;

    QSize getSize();

    QWidget         *centralWidget;
    QScrollArea     *glWidgetArea;
    QScrollArea     *pixmapLabelArea;

    //Menu Pointers
    QMenu           *fileMenu;
    QMenu           *editMenu;
    QMenu           *viewMenu;
    QMenu           *displayMenu;
    QMenu           *shapeMenu;
    QMenu           *toolsMenu;
    QMenu           *selectMenu;
    QMenu           *constructMenu;
    QMenu           *previewMenu;
    QMenu           *windowMenu;
    QMenu           *helpMenu;

    QMenu           *insertMenu;

    //Actions
    QAction         *exitAct;
    QAction         *aboutAct;    

    //these are shape spesific actions
    //need to be moved somewhere else

    QAction *       patchesOnAct;
    QAction *       normalsOnAct;
    QAction *       shadingOnAct;
    QAction *       ambientOnAct;
    QAction *       shadowOnAct;
    QAction *       previewOnAct;
    //QAction *       shapemapOnAct;

    //File Menu Actions
    QAction *       fileNewAct;
    QAction *       fileSaveAct;
    QAction *       fileSaveAsAct;
    QAction *       fileOpenAct;

    //Edit Menu Action
    QAction *       editUndoAct;
    QAction *       editRedoAct;

    //view
    QAction *       viewOptionsAct;
    QAction *       viewAttrAct;


    //Tools Menu Actions
    QAction *       dragAct;
    QAction *       previewAct;

    QAction *       extrudeEdgeAct;
    QAction *       extrudeFaceAct;
    QAction *       insertSegmentAct;
    QAction *       deleteFaceAct;

    //Shape Menu Actions
    QAction *       shapeInsertTorusAct;
    QAction *       shapeInsertEllipseAct;
    QAction *       shapeInsert2NGonAct;
    QAction *       shapeInsertSpineAct;
    QAction *       shapeInsertGridAct;
    QAction *       shapeInsertFacialAct;
    QAction *       shapeInsertImageShapeAct;

    QAction *       shapeLockAct;
    QAction *       shapeMoveFrontAct;
    QAction *       shapeMoveBackAct;
    QAction *       shapeSendFrontAct;
    QAction *       shapeSendBackAct;
    QAction *       shapeParentAct;
    QAction *       shapeTransformAct;
    QAction *       shapeDeleteAct;

    QAction *       assignPatternAct;
    QAction *       setFoldsAct;

    //QAction *       windowSh

    std::map<int, int>      _optionWidgetIDs;
    std::map<void*, int>    _attrWidgetIDs;

    //need to improve this
    struct Options{
        enum OptionWidget_e {NONE, DRAG, ELLIPSE, GRID, NGON, TORUS, SPINE, IMAGE_SHAPE, EXTRUDE, INSERT_SEGMENT, DELETE_FACE, ASSIGN_PATTERN, SET_FOLDS};
       };
};


#endif // MAINWINDOW_H
