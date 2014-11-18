#include <sstream>
#include <qgl.h>
#include <QImage>
#include "fileio.h"
#include "MeshShape/pattern.h"
#include "MeshShape/meshshape.h"
#include "Renderer/imageshape.h"
#include "glwidget.h"
#include "canvas.h"

#define BUCKETS 4
#define SAT_BUCKETS 1

void ISExporter::init(){
    Session::get()->glWidget()->renderPreviewTextures();//first
}

bool ISExporter::exportShape(Shape *, const char *fname){

}

RGB toRGB(QColor& col){
    return RGB(col.redF(), col.greenF(), col.blueF());
}

void findDandB(RGBA *rgba, int &iD, int & iB)
{
    iD = 0;
    iB = BUCKETS-1;
    for(int i=0; i<BUCKETS; i++)
    {
        if (i < BUCKETS*0.25 && rgba[i].w > rgba[iD].w)
                iD = i;

        if (i > BUCKETS*0.75 && rgba[i].w > rgba[iB].w)
                iB = i;
    }
}

void exportBy(const QImage& bg, const QImage& smap)
{
    if (bg.size() != smap.size()) //assert size match
        return;

    Vec3 lp = Session::get()->canvas()->lightPos3D();

    int W = bg.width(), H = bg.height();

    RGBA
            histogram[BUCKETS][SAT_BUCKETS][BUCKETS];

    for(int i_h = 0; i_h < H; i_h++)
    {
        for(int i_w = 0; i_w < W; i_w++)
        {
            //QColor col = QColor(_img.pixel( x - (ww/2) + i,  y - (hh/2) + j ));
            QColor col(bg.pixel(i_w, i_h));
            float
                    h = col.hueF(),
                    s = col.saturationF(),
                    v = col.valueF();

            if (h < 0)
                continue;

            int
                    bi_h = CLAMP((h * BUCKETS), 0 , BUCKETS-1),
                    bi_s = CLAMP((s * SAT_BUCKETS), 0 , SAT_BUCKETS-1),
                    bi_v = CLAMP((v * BUCKETS), 0 , BUCKETS-1);

            histogram[bi_h][bi_s][bi_v] = histogram[bi_h][bi_s][bi_v] + RGBA(col.redF(), col.greenF(), col.blueF(),1);
        }
    }

    int size =  W*H;

    //QImage darkI(W, H, QImage::Format_ARGB32);
    //QImage brightI(W, H, QImage::Format_ARGB32);

    uchar* darkbits   = new uchar[size*4];
    uchar* brightbits = new uchar[size*4];
    uchar* final2bits = new uchar[size*4];

    for(int i_h = 0; i_h < H; i_h++)
    {
        for(int i_w = 0; i_w < W; i_w++)
        {
            //QColor col = QColor(_img.pixel( x - (ww/2) + i,  y - (hh/2) + j ));
            QRgb nrgb = smap.pixel(i_w, i_h);
            QColor coln(nrgb);

            if (qAlpha(nrgb) == 0) continue;

            //Vec3 n(coln.redF(), coln.greenF(), coln.blueF());

            Vec3 n(coln.redF()*2.0-1.0, coln.greenF()*2.0-1.0,0);
            n.z = sqrt(n.x*n.x + n.y*n.y);

            //if (n.norm()>1.0) continue;

            double px = i_w *2.0 / (W-1) - 1.0;
            double py = (H - i_h - 1)*2.0 / (H-1) - 1.0;

            Vec3 lvn = (lp - Vec3(px, py, 0)).normalize();

            float cosa = n * lvn;
            float t = (cosa+1)/2.0;
            t = CLAMP(t, 0.0, 1.0);

            QColor col(bg.pixel(i_w, i_h));

            RGB dark(0, 0, 0);
            RGB bright(1,1,1);
            RGB final(col.redF(), col.greenF(), col.blueF());

            if (col.hueF()>=0)
            {
                int
                        bi_h = col.hueF()*BUCKETS,
                        bi_s = col.saturationF()*SAT_BUCKETS, iD, iB;
                findDandB(histogram[bi_h][bi_s], iD, iB);
                dark.set(histogram[bi_h][bi_s][iD]);
                bright.set(histogram[bi_h][bi_s][iB]);
            }

            //bright = (final-dark*(1.0-t))*(1.0/t);

            RGB final2 = bright*t + dark*(1-t);

            int off = (i_w + i_h*W)*4;

            for(int i=0; i<3; i++)
            {
                float k = final[i]/(final2[i]<0.0001?0.0001:final2[i]);
                darkbits[off+2-i] = CLAMP(dark[i]*k, 0.0, 1.0)*255;
                brightbits[off+2-i] =CLAMP(bright[i]*k, 0.0, 1.0)*255;
                final2bits[off+2-i] = final2[i]*255;
            }

            darkbits[off+3] = 255;
            brightbits[off+3] = 255;
            final2bits[off+3] = 255;
        }
    }

    QImage darkI(darkbits, W, H, QImage::Format_ARGB32);
    QImage brightI(brightbits, W, H, QImage::Format_ARGB32);
    QImage final2I(final2bits, W, H, QImage::Format_ARGB32);

    darkI.save("c:/temp/D.png");
    brightI.save("c:/temp/B.png");
    smap.save("c:/temp/smap.png");
    final2I.save("c:/temp/final2.png");
}

void ISExporter::exportScene(const char *fname)
{
    Session::get()->glWidget()->renderPreviewTextures();

    int w = Session::get()->glWidget()->getRShader()->m_BrightFBO->width(),
        h = Session::get()->glWidget()->getRShader()->m_BrightFBO->height();

    QImage c0 = Session::get()->glWidget()->getRShader()->m_DarkFBO->genQImage();
    QImage c1 = Session::get()->glWidget()->getRShader()->m_BrightFBO->genQImage();

    QImage c2 = Session::get()->glWidget()->getRShader()->m_ShadeFBO->toImage();
    QImage c3 = Session::get()->glWidget()->getRShader()->m_BgFBO->genQImage();
    QImage smap = Session::get()->glWidget()->getRShader()->m_ShapeMapFBO->genQImage();

    exportBy(c3, smap);
    return;

    int size = w*h*4;

    //float* c_k = new float[size];

    const uchar* c0bits = c0.bits();
    const uchar* c1bits = c1.bits();

    const uchar* c2bits = c2.bits();
    const uchar* c3bits = c3.bits();

    uchar* c0bits_r = new uchar[size];
    uchar* c1bits_r = new uchar[size];

    //rectify
    for(int i = 0; i < size; i++)
    {
        if (c2bits[i]==0)
            continue;

        float k = c3bits[i]*1.0 / c2bits[i];
        c0bits_r[i] = CLAMP(k*c0bits[i],0,255);
        c1bits_r[i] = CLAMP(k*c1bits[i],0,255);
    }


    QImage c_0r(c0bits_r, w, h, QImage::Format_ARGB32);
    QImage c_1r(c1bits_r, w, h, QImage::Format_ARGB32);

    c_0r.save("c:/temp/dark_r.png");
    c_1r.save("c:/temp/bright_r.png");

    c0.save("c:/temp/dark.png");
    c1.save("c:/temp/bright.png");
    c2.save("c:/temp/shaded.png");
    smap.save("c:/temp/shapemap.png");

}



/*


void exportBy(const QImage& bg, const QImage& smap)
{
    if (bg.size() != smap.size()) //assert size match
        return;

    Vec3 lp = Session::get()->canvas()->lightPos3D();

    int W = bg.width(), H = bg.height();

    RGB
            darks[HUE_BUCKETS],
            brights[HUE_BUCKETS];

    float
            values[HUE_BUCKETS][2];

    for(int i=0; i<HUE_BUCKETS; i++){
        values[i][0] = 1.0; //dark
        values[i][1] = 0;   //bright
    }

    for(int i_h = 0; i_h < H; i_h++)
    {
        for(int i_w = 0; i_w < W; i_w++)
        {
            //QColor col = QColor(_img.pixel( x - (ww/2) + i,  y - (hh/2) + j ));
            QColor col(bg.pixel(i_w, i_h));
            float
                    h = col.hueF(),
                    s = col.saturationF(),
                    v = col.valueF();

            if (h < 0 || s < 0.01)
                continue;

            int bi = h * 10;
            bi = CLAMP(bi, 0, 9);

            if (values[bi][0] > v/s)
            {
                darks[bi].set(col.redF(), col.greenF(), col.blueF());
                values[bi][0] = v/s;
            }

            if (values[bi][1] < v*s)
            {
                brights[bi].set(col.redF(), col.greenF(), col.blueF());
                values[bi][1] = v*s;
            }
        }
    }

    int size =  W*H;

    //QImage darkI(W, H, QImage::Format_ARGB32);
    //QImage brightI(W, H, QImage::Format_ARGB32);

    uchar* darkbits   = new uchar[size*4];
    uchar* brightbits = new uchar[size*4];

    for(int i_h = 0; i_h < H; i_h++)
    {
        for(int i_w = 0; i_w < W; i_w++)
        {
            //QColor col = QColor(_img.pixel( x - (ww/2) + i,  y - (hh/2) + j ));
            QRgb nrgb = smap.pixel(i_w, i_h);
            QColor coln(nrgb);

            if (qAlpha(nrgb) == 0) continue;

            //Vec3 n(coln.redF(), coln.greenF(), coln.blueF());

            Vec3 n(coln.redF()*2.0-1.0, coln.greenF()*2.0-1.0,0);
            n.z = sqrt(n.x*n.x + n.y*n.y);

            //if (n.norm()>1.0) continue;

            double px = i_w *2.0 / (W-1) - 1.0;
            double py = (H - i_h - 1)*2.0 / (H-1) - 1.0;

            Vec3 lvn = (lp - Vec3(px, py, 0)).normalize();

            float cosa = n * lvn;
            float t = (cosa+1)/2.0;
            t = CLAMP(t, 0.0, 1.0);

            QColor col(bg.pixel(i_w, i_h));

            RGB dark(0, 0, 0);
            RGB bright(1,1,1);
            RGB final(col.redF(), col.greenF(), col.blueF());

            if (col.hueF()>=0)
            {
                int bi = col.hueF()*10;
                dark = darks[bi];//set(darks[bi].redF(), darks[bi].greenF(), darks[bi].blueF());
                bright = brights[bi];//.set(brights[bi].redF(), brights[bi].greenF() ,brights[bi].blueF());
            }

            //bright = (final-dark*(1.0-t))*(1.0/t);

            RGB final2 = bright*t + dark*(1-t);

            int off = (i_w + i_h*W)*4;

            for(int i=0; i<3; i++)
            {
                float k = final2[i]/final[i];
                darkbits[off+2-i] = CLAMP(dark[i]*t + dark[i]*k*(1-t), 0.0, 1.0)*255;
                brightbits[off+2-i] = CLAMP(bright[i]*(1-t) + bright[i]*k*t, 0.0, 1.0)*255;
            }

            darkbits[off+3] = 255;
            brightbits[off+3] = 255;
        }
    }

    QImage darkI(darkbits, W, H, QImage::Format_ARGB32);
    QImage brightI(brightbits, W, H, QImage::Format_ARGB32);

    darkI.save("c:/temp/D.png");
    brightI.save("c:/temp/B.png");
    smap.save("c:/temp/smap.png");
}

*/
