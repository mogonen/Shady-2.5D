#ifndef SYMMETRYQUAD_H
#define SYMMETRYQUAD_H

#include "featurequads.h"

//79 15 14 13 78
//80 84 82 86 77
// 0 83 81 85 12
// 1 50 49 48 11
// 2 58 56 54 10
// 3 59 87 65  9
// 4  5  6  7  8

const int Sym_connectivity[] =
{
    3,4,5,59,
    5,6,87,59,
    6,7,65,87,
    7,8,9,65,
    2,3,59,58,
    59,87,56,58,
    87,65,54,56,
    65,9,10,54,
    2,58,50,1,
    58,56,49,50,
    56,54,48,49,
    54,10,11,48,
    1,50,83,0,
    50,49,81,83,
    49,48,85,81,
    48,11,12,85,
    0,83,84,80,
    83,81,82,84,
    81,85,86,82,
    85,12,77,86,
    80,84,15,79,
    84,82,14,15,
    82,86,13,14,
    86,77,78,13
};

class SymmetryQuad : public FeatureQuads
{
public:
    SymmetryQuad(const float *landMarks, int num_lm, int w, int h);
    SymmetryQuad();
    void BuildInnterPoints();
    void AddAuxiliaryPoints();
    bool Intersect(cv::Point2d A1, cv::Point2d A2, cv::Point2d B1, cv::Point2d B2, float *out);
    void AssignNormalColor();
    void GetColors(float **colors, int &num);
    std::vector<cv::Vec3f> GetColorVector(){return m_NormalColor;}
    static void PrintLandmarks(const float* landmarks, const char* msg);
    static void Exit(const char* format, ...);
    std::vector<std::vector<int> > CreatGridIndices();
    std::string GetFileName(){return m_fileName;}

    //For line segment a-b-c, get normal direction at b
    cv::Vec3f CalNormalColor(cv::Point2d a, cv::Point2d b, cv::Point2d c);
    std::vector<cv::Vec3f> m_NormalColor;
    std::vector<std::vector<int> > m_gridIndex;
    int m_width;
    int m_height;
    std::string m_fileName;
};

#endif // SYMMETRYQUAD_H
