#include "symmetryquad.h"
#include <iostream>
#include <stdarg.h>
using namespace std;

SymmetryQuad::SymmetryQuad(const float *landMarks, int num_lm, int w, int h)
{
    this->SetVertices(landMarks, num_lm,Sym_connectivity,24);
    AddAuxiliaryPoints();
    m_width = w;
    m_height = h;
}


SymmetryQuad::SymmetryQuad()
{
    string path = "D:/youyou_program/ASM/faces/male/001.jpg";
    m_fileName = path;
    const cv::Mat_<unsigned char> img(cv::imread(path, CV_LOAD_IMAGE_GRAYSCALE));

    m_width = img.cols;
    m_height = img.rows;
    float landmarks[2 * stasm_NLANDMARKS]; // x,y coords
    int foundface;
    float estyaw;
    if (!stasm_init("../data", 0))
        Exit("stasm_init failed: %s", stasm_lasterr());
    stasm_open_image((const char*)img.data, img.cols, img.rows,
                     path.data(), true, 55);
    if (!stasm_search_auto_ext(&foundface, landmarks, &estyaw))
        Exit("stasm_init failed: %s", stasm_lasterr());
    char s[100]; sprintf(s, "\nFinal with auto init (estyaw %.0f)", estyaw);

    this->SetVertices(landmarks, stasm_NLANDMARKS,Sym_connectivity,24);
    AddAuxiliaryPoints();
}

void SymmetryQuad::AddAuxiliaryPoints()
{
    float int_p;
    Intersect(m_vec2Vertices[11],m_vec2Vertices[12],m_vec2Vertices[13],m_vec2Vertices[14],&int_p);
    cv::Point2d intersection = ((m_vec2Vertices[14] - m_vec2Vertices[13]) * int_p) + m_vec2Vertices[13];
    intersection = intersection*(6.0f/8.0f)+m_vec2Vertices[13]*(1.0/8.0)+m_vec2Vertices[12]*(1.0/8.0);
    cv::Point2d intint1 = (intersection+m_vec2Vertices[12])*0.5;
    cv::Point2d intint2 = (intersection+m_vec2Vertices[13])*0.5;

    Intersect(m_vec2Vertices[0],m_vec2Vertices[1],m_vec2Vertices[14],m_vec2Vertices[15],&int_p);
    intersection = ((m_vec2Vertices[15] - m_vec2Vertices[14]) * int_p) + m_vec2Vertices[14];
    intersection = intersection*(6.0f/8.0f)+m_vec2Vertices[0]*(1.0/8.0)+m_vec2Vertices[15]*(1.0/8.0);
    cv::Point2d intint3 = (intersection+m_vec2Vertices[15])*0.5;
    cv::Point2d intint4 = (intersection+m_vec2Vertices[0])*0.5;

    m_vec2Vertices.push_back(intint1);
    m_vec2Vertices.push_back(intint2);
    m_vec2Vertices.push_back(intint3);
    m_vec2Vertices.push_back(intint4);

    m_vec2Vertices.push_back((m_vec2Vertices[30]+m_vec2Vertices[40])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[21]+m_vec2Vertices[22])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[81]+m_vec2Vertices[30])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[82]+m_vec2Vertices[21])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[81]+m_vec2Vertices[40])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[82]+m_vec2Vertices[22])*0.5);
    m_vec2Vertices.push_back((m_vec2Vertices[67]+m_vec2Vertices[70])*0.5);
}

bool SymmetryQuad::Intersect(cv::Point2d A1, cv::Point2d A2, cv::Point2d B1, cv::Point2d B2, float *out)
{
    cv::Point2d a(A2-A1);
    cv::Point2d b(B2-B1);

    double f = a.y*b.x-a.x*b.y;
    if(!f)      // lines are parallel
        return false;

    cv::Point2d c(B2-A2);
    double aa = a.y*c.x-a.x*c.y;
    double bb = b.y*c.x-b.x*c.y;

    *out = 1.0 - (aa / f);
    return true;
}


void SymmetryQuad::AssignNormalColor()
{
//    std::vector<cv::Point2d> outer_contour;
//    outer_contour.insert(outer_contour.begin(), this->m_vec2Vertices.begin(),this->m_vec2Vertices.begin()+13);
//    cv::Point* elementPoints[1] = { &outer_contour[0] };
//    int num_p = outer_contour.size();
//    cv::fillPoly( img, elementPoints,num_p, cv::Scalar(128));

      m_NormalColor = std::vector<cv::Vec3f>(m_vec2Vertices.size(),cv::Vec3f(0.5,0.5,1.0));
      m_NormalColor[0] = CalNormalColor(m_vec2Vertices[80],m_vec2Vertices[0],m_vec2Vertices[1]);
      m_NormalColor[80] = CalNormalColor(m_vec2Vertices[79],m_vec2Vertices[80],m_vec2Vertices[0]);
      m_NormalColor[79] = CalNormalColor(m_vec2Vertices[15],m_vec2Vertices[79],m_vec2Vertices[80]);
      m_NormalColor[15] = CalNormalColor(m_vec2Vertices[14],m_vec2Vertices[15],m_vec2Vertices[79]);
      m_NormalColor[14] = CalNormalColor(m_vec2Vertices[13],m_vec2Vertices[14],m_vec2Vertices[15]);
      m_NormalColor[13] = CalNormalColor(m_vec2Vertices[78],m_vec2Vertices[13],m_vec2Vertices[14]);
      m_NormalColor[78] = CalNormalColor(m_vec2Vertices[77],m_vec2Vertices[78],m_vec2Vertices[13]);
      m_NormalColor[77] = CalNormalColor(m_vec2Vertices[12],m_vec2Vertices[77],m_vec2Vertices[78]);
      m_NormalColor[12] = CalNormalColor(m_vec2Vertices[11],m_vec2Vertices[12],m_vec2Vertices[77]);
      m_NormalColor[59] = CalNormalColor(m_vec2Vertices[57],m_vec2Vertices[59],m_vec2Vertices[87]);
      m_NormalColor[65] = CalNormalColor(m_vec2Vertices[55],m_vec2Vertices[65],m_vec2Vertices[87]);
//      m_NormalColor[87] = CalNormalColor(m_vec2Vertices[59],m_vec2Vertices[87],m_vec2Vertices[65]);


      m_NormalColor[58] = CalNormalColor(m_vec2Vertices[50],m_vec2Vertices[58],m_vec2Vertices[59])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[54] = CalNormalColor(m_vec2Vertices[48],m_vec2Vertices[54],m_vec2Vertices[65])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[50] = -CalNormalColor(m_vec2Vertices[83],m_vec2Vertices[50],m_vec2Vertices[58])+cv::Vec3f(1.0,1.0,0.5);
      m_NormalColor[48] = -CalNormalColor(m_vec2Vertices[85],m_vec2Vertices[48],m_vec2Vertices[54])+cv::Vec3f(1.0,1.0,0.5);

      m_NormalColor[83] = CalNormalColor(m_vec2Vertices[82],m_vec2Vertices[83],m_vec2Vertices[49])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[85] = CalNormalColor(m_vec2Vertices[82],m_vec2Vertices[85],m_vec2Vertices[49])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[84] = CalNormalColor(m_vec2Vertices[82],m_vec2Vertices[84],m_vec2Vertices[83])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[86] = CalNormalColor(m_vec2Vertices[82],m_vec2Vertices[86],m_vec2Vertices[85])+cv::Vec3f(0.0,0.0,0.5);
      m_NormalColor[82] = (CalNormalColor(m_vec2Vertices[84],m_vec2Vertices[82],m_vec2Vertices[86])+m_NormalColor[14])/2+cv::Vec3f(0.0,0.0,0.5);

      for(int i=1;i<12;i++)
      {
          m_NormalColor[i] = CalNormalColor(m_vec2Vertices[i-1],m_vec2Vertices[i],m_vec2Vertices[i+1]);
      }
//      std::cout<<m_NormalColor<<std::endl;
//      m_NormalColor.at<cv::Scalar>(1,0) = CalNormalColor(m_vec2Vertices[0],m_vec2Vertices[1],m_vec2Vertices[2]);
//        std::cout<<m_NormalColor;
//    for(int i=1;i<11;i++)
//    {


//    }

}


cv::Vec3f SymmetryQuad::CalNormalColor(cv::Point2d a, cv::Point2d b, cv::Point2d c)
{
    cv::Point2d diff = b-a;
    cv::Vec2f ba = cv::normalize(cv::Vec2f(diff.x,diff.y));
    diff = b-c;
    cv::Vec2f bc = cv::normalize(cv::Vec2f(diff.x,diff.y));
    cv::Vec2f s_normal=cv::normalize(ba+bc);
    return cv::Vec3f(s_normal.val[0]/2+0.5,-s_normal.val[1]/2+0.5,0.0);
}



void SymmetryQuad::GetColors(float **colors, int &num)
{
    num = m_vec2Vertices.size();
    (*colors) = new float[num*3];
    for(int i=0;i<num;i++)
    {
        for(int j=0;j<3;j++)
            (*colors)[3*i+j] = (m_NormalColor[i])[j];
    }
}



void SymmetryQuad::PrintLandmarks(const float* landmarks, const char* msg)
{
    stasm_printf("%s:\n", msg);
    for (int i = 0; i < stasm_NLANDMARKS; i++)
        stasm_printf("%3d: %4.0f %4.0f\n",
                     i, landmarks[i*2], landmarks[i*2+1]);
}
void SymmetryQuad::Exit(const char* format, ...) // args like printf
{
    char s[1024+1];
    va_list args;
    va_start(args, format);
    vsprintf(s, format, args);
    va_end(args);
    stasm_printf("\n%s\n", s);
    exit(1);
}

std::vector<std::vector<int> > SymmetryQuad::CreatGridIndices()
{
    if(m_gridIndex.size()==0)
    {
        m_gridIndex.resize(5);
        for(int i=0;i<5;i++)
            m_gridIndex[i].resize(7);
        //column 0
        for(int i=0;i<=4;i++)
            m_gridIndex[0][i] = 4-i;
        m_gridIndex[0][5] = 80;
        m_gridIndex[0][6] = 79;
        //column 1
        m_gridIndex[1][0] = 5;
        m_gridIndex[1][1] = 59;
        m_gridIndex[1][2] = 58;
        m_gridIndex[1][3] = 50;
        m_gridIndex[1][4] = 83;
        m_gridIndex[1][5] = 84;
        m_gridIndex[1][6] = 15;
        //column 2
        m_gridIndex[2][0] = 6;
        m_gridIndex[2][1] = 87;
        m_gridIndex[2][2] = 56;
        m_gridIndex[2][3] = 49;
        m_gridIndex[2][4] = 81;
        m_gridIndex[2][5] = 82;
        m_gridIndex[2][6] = 14;
        //column 3
        m_gridIndex[3][0] = 7;
        m_gridIndex[3][1] = 65;
        m_gridIndex[3][2] = 54;
        m_gridIndex[3][3] = 48;
        m_gridIndex[3][4] = 85;
        m_gridIndex[3][5] = 86;
        m_gridIndex[3][6] = 13;
        //column 4
        for(int i=0;i<=4;i++)
            m_gridIndex[4][i] = 8+i;
        m_gridIndex[4][5] = 77;
        m_gridIndex[4][6] = 78;
    }
    return m_gridIndex;
}

