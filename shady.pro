CONFIG +=

HEADERS += glwidget.h \
    mainwindow.h \
    vec/Vector.h \
    vec/Utility.h \
    vec/Quaternion.h \
    vec/Matrix.h \
    base.h \
    canvas.h \
    sampleshape.h \
    controlpoint.h \
    curve.h \
    customdialog.h \
    shape.h \
    FacialShape/facialshape.h \
    FacialShape/symmetryquad.h \
    FacialShape/featurequads.h \
    meshshape/patch.h \
    meshshape/spineshape.h \
    meshshape/meshshape.h \
    meshshape/meshdata.h \
    shapecontrol.h \
    ellipseshape.h \
    Renderer/shaderprogram.h \
    Renderer/shaderparameters.h \
    FacialShape/shadingcolorextractor.h \
    meshshape/cmesh.h \
    Renderer/qsliderspinbox.h \
    Renderer/renderoptionspenal.h \
    Renderer/imageshape.h

SOURCES += glwidget.cpp \
    main.cpp \
    mainwindow.cpp \
    vec/Vector.cpp \
    vec/Utility.cpp \
    vec/Quaternion.cpp \
    vec/Matrix.cpp \
    render.cpp \
    curve.cpp \
    customdialog.cpp \
    shape.cpp \
    FacialShape/facialshape.cpp \
    FacialShape/symmetryquad.cpp \
    FacialShape/featurequads.cpp \
    meshshape/patch.cpp \
    meshshape/spine.cpp \
    meshshape/meshshape.cpp \
    meshshape/cmesh.cpp \
    meshshape/beziershape.cpp \
    meshshape/meshoperations.cpp \
    meshshape/meshprimitives.cpp \
    uicalbacks_meshshape.cpp \
    shapecontrol.cpp \
    Renderer/shaderprogram.cpp \
    Renderer/shaderparameters.cpp \
    FacialShape/shadingcolorextractor.cpp \
    glwidget_render.cpp \
    glwidget_canvas.cpp \
    selection.cpp \
    session.cpp \
    Renderer/qsliderspinbox.cpp \
    Renderer/renderoptionspenal.cpp \
    Renderer/imageshape.cpp

QT           += opengl widgets



FACIAL_SHAPE{
    DEFINES += FACIAL_SHAPE
    OPENCV_HOME=D:\opencv246
    OPENCV_V = 246
    STASM_HOME = D:\youyou_program\ASM\stasm4.0.0
    STASM_LIB = stasm_lib_proj

    INCLUDEPATH += -I $${OPENCV_HOME}\build\include \
    -I $${STASM_HOME}\stasm

    LIBS += -L$${OPENCV_HOME}\mingw\bin \
    -lopencv_core$${OPENCV_V} \
    -lopencv_highgui$${OPENCV_V} \
    -lopencv_imgproc$${OPENCV_V} \
    -lopencv_objdetect$${OPENCV_V} \
    -L$${STASM_HOME}\build\mingw48_32\bin \
    -l$${STASM_LIB}
}else{
    HEADERS -=    FacialShape/facialshape.h \
    FacialShape/symmetryquad.h \
    FacialShape/featurequads.h \
    FacialShape/shadingcolorextractor.h
    SOURCES -=     FacialShape/facialshape.cpp \
    FacialShape/symmetryquad.cpp \
    FacialShape/featurequads.cpp \
    FacialShape/shadingcolorextractor.cpp
}

RESOURCES += \
    Renderer/Shaders.qrc

OTHER_FILES += \
    Renderer/Basic.vsh \
    Renderer/Basic.fsh \
    Renderer/Modelling.fsh

