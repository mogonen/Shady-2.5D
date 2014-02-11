#include "ywrenderer.h"
#include "canvas.h"
#include "glwidget.h"

YWRenderer::YWRenderer():Renderer(Session::get()->canvas()){

}

void YWRenderer::init(){
    _pGLSLShader_R = new ShaderProgram();
    _pGLSLShader_R->Initialize();
    _pGLSLShader_M = new ShaderProgram(ShaderProgram::TYPE_MODEL);
    _pGLSLShader_M->Initialize();
}

void YWRenderer::render(int mode)
{
    _pGLSLShader_R->release();
    glReadBuffer(GL_BACK);

    ShapeList shapelist = Session::get()->canvas()->shapes();
    int width = Session::get()->glWidget()->width();
    int height = Session::get()->glWidget()->height();


    if(!_pGLSLShader_R->isInitialized())
    {
        Session::get()->deactivate();
        _pGLSLShader_R->SetInitialized(true);
        FOR_ALL_CONST_ITEMS(ShapeList, shapelist)
        { //need to add layers
            Shape_p pShape = *it;
            glPushMatrix();
            Point p = pShape->P();
            glTranslatef(p.x, p.y, 0);
            /*apply(pShape->getTransform());
            glMultMatrixd((double*)&tM);*/
            render(pShape,SM_MODE);
            glPopMatrix();
        }

        _pGLSLShader_R->GrabShapeMap(width, height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        FOR_ALL_CONST_ITEMS(ShapeList, shapelist)
        { //need to add layers
            Shape_p pShape = *it;
            glPushMatrix();
            Point p = pShape->P();
            glTranslatef(p.x, p.y, 0);
            /*apply(pShape->getTransform());
            glMultMatrixd((double*)&tM);*/
            pShape->type();
            render(pShape, BRIGHT_MODE);
            glPopMatrix();
        }
        _pGLSLShader_R->GrabBrightMap();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        FOR_ALL_CONST_ITEMS(ShapeList, shapelist){ //need to add layers
            Shape_p pShape = *it;
            glPushMatrix();
            Point p = pShape->P();
            glTranslatef(p.x, p.y, 0);
            /*apply(pShape->getTransform());
            glMultMatrixd((double*)&tM);*/
            pShape->type();
            render(pShape, DARK_MODE);
            glPopMatrix();
        }
        _pGLSLShader_R->GrabDarkMap();

        Session::get()->canvas()->updateDepth();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        FOR_ALL_CONST_ITEMS(ShapeList, shapelist){ //need to add layers
            Shape_p pShape = *it;
            glPushMatrix();
            Point p = pShape->P();
            glTranslatef(p.x, p.y, 0);
            /*apply(pShape->getTransform());
            glMultMatrixd((double*)&tM);*/
            render(pShape, LABELDEPTH_MODE);
            glPopMatrix();
        }
        _pGLSLShader_R->GrabLDMap();

        _pGLSLShader_R->SetTextureToShader();
    }

    _pGLSLShader_R->bind();
    _pGLSLShader_R->LoadShaperParameters(shapelist);
    _pGLSLShader_R->setUniformValue("width", (float)width);
    _pGLSLShader_R->setUniformValue("height", (float)height);
    _pGLSLShader_R->setUniformValue("toggle_ShaAmbCos", (int)(_renderFlags>>1)&7);
    float aspect = (float)height / width;


    glBegin(GL_QUADS);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glTexCoord2d(0.0,0.0);
    glVertex2f(-1.0,-1.0);
    glTexCoord2d(0.0,1.0);
    glVertex2f(-1.0,1.0);
    glTexCoord2d(1.0,1.0);
    glVertex2f(1.0,1.0);
    glTexCoord2d(1.0,0.0);
    glVertex2f(1.0,-1.0);
    glEnd();

    _pGLSLShader_R->release();
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);

    if (is(DRAGMODE_ON))
    {
        glColor3f(1.0, 1.0, 0);
        Session::get()->canvas()->light(0)->render(DRAG_MODE);
    }

    glColor3f(1.0, 1.0, 0);
    QFont m_font;
    m_font.setPointSize(15);
    QVector3D mousePos = _pGLSLShader_R->m_MousePos;
    QString z_pos = QString("pos: %1 %2 %3").arg(mousePos.x(),0,'f',3).arg(mousePos.y(),0,'f',3).arg(mousePos.z(),0,'f',3);
    Session::get()->glWidget()->renderText(10, 20, z_pos, m_font);
}

void YWRenderer::reloadShader(){
    _pGLSLShader_R->ReloadShader();_pGLSLShader_M->ReloadShader();
}


void YWRenderer::render(Shape_p pShape, int mode){
    pShape->render(mode);
}

void YWRenderer::render(ImageShape* pIShape, int mode){
    pIShape->render(mode);
}

void ImageShape::render(int mode)
{
    YWRenderer* pRenderer = ((YWRenderer*)Session::get()->renderer(1)); //probably needs a better solution
    Shape::render(mode);
    if(m_texUpdate!=NO_UPDATE)
        InitializeTex();
    if(mode&DEFAULT_MODE||mode&DRAG_MODE&&!(mode&SM_MODE||mode&DARK_MODE||mode&BRIGHT_MODE||mode&LABELDEPTH_MODE))
    {
        switch(m_curTexture)
        {
        case 0:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            break;
        case 1:
            glBindTexture(GL_TEXTURE_2D, m_texDark);
            break;
        case 2:
            glBindTexture(GL_TEXTURE_2D, m_texBright);
            break;
        case 3:
            glBindTexture(GL_TEXTURE_2D, m_texSM);
            mode = mode|LABELDEPTH_MODE;
            break;
        }
    }
    else
        if(mode&SM_MODE||mode&LABELDEPTH_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texSM);
        else if(mode&DARK_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texDark);
        else if(mode&BRIGHT_MODE)
            glBindTexture(GL_TEXTURE_2D, m_texBright);




    pRenderer->getMShader()->bind();
    pRenderer->getMShader()->setUniformValue("alpha_th", (float)m_alpha_th);
    if(mode&LABELDEPTH_MODE)
        pRenderer->getMShader()->setUniformValue("isLabelDepth", (float)1.0);
    else
        pRenderer->getMShader()->setUniformValue("isLabelDepth", (float)0.0);
//    pRenderer->getMShader()->setUniformValue("label_depth", (float)m_alpha_th);


    int total_num = Session::get()->canvas()->getNumShapes();
    qDebug()<<"normal"<<_shaderParam.m_averageNormal;
//    glDepthMask(GL_FALSE);
    double delta_LB2RT = -(_shaderParam.m_averageNormal.x()*m_width+_shaderParam.m_averageNormal.y()*m_height)*m_stretch;
    double delta_LT2BR = -(_shaderParam.m_averageNormal.x()*m_width-_shaderParam.m_averageNormal.y()*m_height)*m_stretch;
    double center_depth = this->m_assignedDepth;//(float)(_shaderParam.m_layerLabel+1)/(total_num+1);


    qDebug()<<"_layerLabel"<<(int)_shaderParam.m_layerLabel;
    qDebug()<<"total_num"<<(int)total_num;
    qDebug()<<"center_depth"<<center_depth;
    qDebug()<<"delta_LB2RT"<<delta_LB2RT;
    qDebug()<<"delta_LT2BR"<<delta_LT2BR;




//    if(_shaderParam.m_averageNormal.z()<0)
//    {
//        delta_LB2RT =-delta_LB2RT;
//        delta_LT2BR =-delta_LT2BR;
//    }




    //1.1 is for numerical issue


    float blf = (center_depth-delta_LB2RT);
    blf = CapValue(blf,0,1);
    float tlf = (center_depth-delta_LT2BR);
    tlf = CapValue(tlf,0,1);
    float trf = (center_depth+delta_LB2RT);
    trf = CapValue(trf,0,1);
    float brf = (center_depth+delta_LT2BR);
    brf = CapValue(brf,0,1);


    glBegin(GL_QUADS);
    glColor4f(center_depth,blf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(0.0,0.0);
    glVertex3f(-m_width,-m_height,blf);
    QVector3D bl(-m_width,-m_height,blf);
//    glVertex3f(-m_width,-m_height,0.5);


    glColor4f(center_depth,tlf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(0.0,1.0);
    glVertex3f(-m_width,m_height,tlf);
    QVector3D tl(-m_width,m_height,tlf);
//    glVertex3f(-m_width,m_height,0.5);


    glColor4f(center_depth,trf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0,1.0);
    glVertex3f(m_width,m_height,trf);
    QVector3D tr(m_width,m_height,trf);


//    glVertex3f(m_width,m_height,0.5);


    glColor4f(center_depth,brf,((float)_shaderParam.m_layerLabel+1.01)/255.0,1.0);
    glTexCoord2d(1.0,0.0);
    glVertex3f(m_width,-m_height,brf);
    QVector3D br(m_width,-m_height,brf);
    glEnd();


    QVector3D n1 = QVector3D::crossProduct(br-bl,tl-bl);
//    QVector3D n2 = QVector3D::crossProduct(tr-br,tr-tl);
//    if(n1.z()>0)
//        _shaderParam.m_trueNormal = n1.normalized();
//    else
//        _shaderParam.m_trueNormal = -n1.normalized();


    qDebug()<<"bl"<<blf<<"tl"<<tlf<<"tr"<<trf<<"br"<<brf;
    qDebug()<<"bl"<<bl<<"tl"<<tl<<"tr"<<tr<<"br"<<br;


    //    qDebug()<<"n1"<<n1;
//    qDebug()<<"n2"<<n2;


//    glVertex3f(m_width,-m_height,0.5);


//    glDepthMask(GL_TRUE);
    pRenderer->getMShader()->release();
    glDisable(GL_TEXTURE_2D);
    _shaderParam.m_trueNormal = QVector3D(n1.x(),n1.y(), n1.z()).normalized();
    _shaderParam.m_centerDepth = QVector3D(m_width+P().x,-m_height+P().y,brf);
    _shaderParam.m_boundingbox[0] = -m_width+P().x;
    _shaderParam.m_boundingbox[1] = m_width+P().x;
    _shaderParam.m_boundingbox[2] = -m_height+P().y;
    _shaderParam.m_boundingbox[3] = m_height+P().y;
    _shaderParam.m_shadowcreator = this->m_shadowCreator;
//    glMatrixMode(GL_MODELVIEW);
//    glLoadIdentity();
//    glBegin(GL_POINTS);
//    glColor3f(1.0,1.0,0.0);
//    glVertex3f(_shaderParam.m_centerDepth.x(),_shaderParam.m_centerDepth.y(),0.0);
//    glEnd();


}


float ImageShape::CapValue(float in_num, float low_cap, float high_cap)
{
    if(in_num<low_cap)
        return low_cap;
    if(in_num>high_cap)
        return high_cap;
    return in_num;
}


void ImageShape::InitializeTex()
{
    QImage loadedImage;
    if(m_texUpdate&UPDATE_SM)
    {
        if(glIsTexture(m_texSM))
            Session::get()->glWidget()->deleteTexture(m_texSM);
        if(m_SMFile.isEmpty()||!loadedImage.load(m_SMFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(0.0,0.0,255.0,255.0));
            getShaderParam().m_averageNormal = QVector2D(0.0,0.0);
        }
        else
        {
            int w = loadedImage.width();
            int h = loadedImage.height();
            if(w>h)
                m_height = m_width*h/w;
            else
                m_width = m_height*w/h;
            m_SMimg = loadedImage;
            calAverageNormal();
        }
        m_texSM = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
        if(GetPenal())
            GetPenal()->SetNewSize(m_width,m_height);
    }

    if(m_texUpdate&UPDATE_DARK)
    {
        if(glIsTexture(m_texDark))
            Session::get()->glWidget()->deleteTexture(m_texDark);
        if(m_DarkFile.isEmpty()||!loadedImage.load(m_DarkFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(0.0,0.0,0.0,255.0));
        }
        m_texDark = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }

    if(m_texUpdate&UPDATE_BRIGHT)
    {
        if(glIsTexture(m_texBright))
            Session::get()->glWidget()->deleteTexture(m_texBright);
        if(m_BrightFile.isEmpty()||!loadedImage.load(m_BrightFile))
        {
            loadedImage = QImage(1,1,QImage::Format_ARGB32);
            loadedImage.setPixel(0,0,qRgba(255.0,255.0,255.0,255.0));
        }
        m_texBright = Session::get()->glWidget()->bindTexture(loadedImage, GL_TEXTURE_2D);
    }
    m_texUpdate = NO_UPDATE;
}

void YWRenderer::updateGLSLLight(float x, float y, float z)
{
    _pGLSLShader_R->bind();
    _pGLSLShader_R->SetLightPos(QVector3D(x,y,z));
    _pGLSLShader_R->release();
}

#ifdef FACIAL_SHAPE
void FacialShape::initBG(){
    QImage img_data = QGLWidget::convertToGLFormat(QImage(QString::fromStdString(m_imgName)));
    glGenTextures(1, &tt);
    glBindTexture(GL_TEXTURE_2D, tt);
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,
                  0, 0,
                  0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
}
void FacialShape::render() const{
    MeshShape::render();
//    if(texture[0])
//    {
//        glEnable(GL_TEXTURE_2D);
//        glBegin(GL_QUADS);
//        glColor4f(1.0, 1.0, 1.0, 1.0);
//        glTexCoord2d(0.0,0.0);
//        glVertex3f(-1.0,-1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(0.0,1.0);
//        glVertex3f(-1.0,1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(1.0,1.0);
//        glVertex3f(1.0,1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glTexCoord2d(1.0,0.0);
//        glVertex3f(1.0,-1.0,0);
//        glNormal3f(0.0,0.0,1.0);
//        glEnd();
//        glDisable(GL_TEXTURE_2D);
//    }
}
#endif
