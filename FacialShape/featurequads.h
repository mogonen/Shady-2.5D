#ifndef FEATUREQUADS_H
#define FEATUREQUADS_H


#include "opencv/highgui.h" // needed for imread
#include "opencv2/imgproc/imgproc.hpp" // needed for imread
#include "stasm_lib.h"
#include "stasm_lib_ext.h"  // needed for stasm_search_auto_ext
#include "stasm_landmarks.h"


class FeatureQuads
{
public:
    FeatureQuads(){;}
    void SetVertices(const std::vector<cv::Point2i> &vertices, const int *connectivities = NULL, int num_quads = 0);
    void SetVertices(const float *landmarks, int num_lm, const int *connectivities = NULL, int num_quads = 0);
    void SetConnectivities(const int *connectivities, int num_quads);
    void DrawOn(cv::Mat &img, bool withNum = false);
    void DrawQuad(const int* quad, cv::Mat &img);
    void GetVertices(float **vertices, int &num);
    void GetIndices(int **indices, int &num);
    std::vector<cv::Point2i> GetVerticesVector() const {return m_vec2Vertices;}
    cv::Point2i Interpolate(cv::Point2i start, cv::Point2i end, float portion);
    const int *m_quadConnectivity;
    std::vector<cv::Point2i> m_vec2Vertices;
    int m_NumQuads;
};

#endif // FEATUREQUADS_H
