#ifndef YWRENDERER_H
#define YWRENDERER_H

#include "../base.h"
#include "shaderprogram.h"
#include "imageshape.h"


enum YWRenderMode {DEFAULT_MODE = 0, DRAG_MODE = 1, SM_MODE = 2, DARK_MODE = 4, BRIGHT_MODE = 8, LABELDEPTH_MODE = 16};

class YWRenderer: public Renderer
{

    ShaderProgram       *_pGLSLShader_R;
    ShaderProgram       *_pGLSLShader_M;

public:

    YWRenderer();

    void                init();

    void                render(int mode);

    void                render(Shape_p pShape, int mode);
    void                render(ImageShape* pIShape, int mode);


    void                reloadShader();
    void                updateGLSLLight(float x, float y, float z);

    ShaderProgram      *getRShader() const {return _pGLSLShader_R;}
    ShaderProgram      *getMShader() const {return _pGLSLShader_M;}

};


#endif // YWRENDERER_H
