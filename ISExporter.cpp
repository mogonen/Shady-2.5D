#include <sstream>
#include <qgl.h>
#include <QImage>
#include "fileio.h"
#include "MeshShape/pattern.h"
#include "MeshShape/meshshape.h"
#include "Renderer/imageshape.h"
#include "glwidget.h"

bool ISExporter::exportShape(Shape *, const char *fname){

}

void ISExporter::exportScene(const char *fname){
    Session::get()->glWidget()->renderPreviewTextures();

    int w = Session::get()->glWidget()->getRShader()->m_BrightFBO->width(),
        h = Session::get()->glWidget()->getRShader()->m_BrightFBO->height();

    /*BitMap_p pC0    = new BitMap(w, h, 4, Session::get()->glWidget()->getRShader()->m_DarkFBO->getData());
    BitMap_p pC1    = new BitMap(w, h, 4, Session::get()->glWidget()->getRShader()->m_BrightFBO->getData());
    BitMap_p pCSM   = new BitMap(w, h, 4, Session::get()->glWidget()->getRShader()->m_ShapeMapFBO->getData());
    //BitMap_p pCout  = new BitMap(w, h, 4, Session::get()->glWidget()->getRShader()->m_ShadeFBO->getData());*/

    QImage* img = Session::get()->glWidget()->getRShader()->m_ShapeMapFBO->genQImage();
    img->save("c:/temp/test.png");
    delete img;
}
