#ifndef SHADINGCOLOREXTRACTOR_H
#define SHADINGCOLOREXTRACTOR_H

#include "symmetryquad.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

class ShadingColorExtractor
{
public:
    ShadingColorExtractor(const SymmetryQuad &sq);
    cv::Mat SkinColorSegment();
    void SampleLandMark();
    cv::Mat BuildHSVImage(std::vector<cv::Mat> in_img, float  HSV_color, int channel = 2, bool mode = true);
    void ColorAnalysis(cv::Mat &in_img, cv::Mat &mask, double &minValue, double &maxValue, double per = 0.01);
    cv::Mat cvtTo4Channle(cv::Mat source, cv::Mat Mask);

    cv::Point2i CalLightingPos(cv::Mat in_value, cv::Mat mask);
    cv::Mat m_darkImage;
    cv::Mat m_brightImage;
    cv::Mat m_orgImage;
    std::vector<cv::Point2i> m_vec2Vertices;
};

#endif // SHADINGCOLOREXTRACTOR_H
