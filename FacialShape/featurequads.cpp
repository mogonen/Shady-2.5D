#include "featurequads.h"

void FeatureQuads::SetVertices(const std::vector<cv::Point2d> &vertices, const int *connectivities, int num_quads)
{
    m_vec2Vertices = vertices;
    SetConnectivities(connectivities, num_quads);
}

void FeatureQuads::SetVertices(const float *landmarks, int num_lm, const int *connectivities, int num_quads)
{
    m_vec2Vertices.reserve(num_lm);
    for(int i=0;i<num_lm;i++)
        m_vec2Vertices.push_back(cv::Point2d(landmarks[2*i],landmarks[2*i+1]));
    SetConnectivities(connectivities, num_quads);
}


void FeatureQuads::SetConnectivities(const int *connectivities, int num_quads)
{
    m_quadConnectivity = connectivities;
    m_NumQuads = num_quads;
}

void FeatureQuads::DrawOn(cv::Mat &img, bool withNum)
{
    for(int i=0;i<m_NumQuads;i++)
        DrawQuad(m_quadConnectivity+i*4, img);
    for(float i=0;i<m_vec2Vertices.size();i++)
    {
        cv::circle(img,
                 m_vec2Vertices[i],2,
                cv::Scalar(0,255,0), -1);
        if(withNum)
        {
            std::ostringstream convert;
            convert << i;
            cv::putText(img, convert.str(), m_vec2Vertices[i], cv::FONT_HERSHEY_SIMPLEX ,
                        0.3, cv::Scalar(0,0,255),0.5);
        }
    }
}

void FeatureQuads::DrawQuad(const int* quad, cv::Mat &img)
{
    for(int i=0;i<3;i++)
    {
        cv::line(img,
                 cv::Point(m_vec2Vertices[*(quad+i)].x, m_vec2Vertices[*(quad+i)].y),
                cv::Point(m_vec2Vertices[*(quad+i+1)].x, m_vec2Vertices[*(quad+i+1)].y),
                cv::Scalar(255,0,0),
                1);
    }
    cv::line(img,
             cv::Point(m_vec2Vertices[*(quad)].x, m_vec2Vertices[*(quad)].y),
            cv::Point(m_vec2Vertices[*(quad+3)].x, m_vec2Vertices[*(quad+3)].y),
            cv::Scalar(255,0,0),
            1);

    for(float i=0;i<m_vec2Vertices.size();i++)
    {
        cv::circle(img,
                 cv::Point(m_vec2Vertices[i].x, m_vec2Vertices[i].y),2,
                cv::Scalar(0,255,0), -1);
    }
}

cv::Point2d FeatureQuads::Interpolate(cv::Point2d start, cv::Point2d end, float portion)
{
    return start+(end-start)*portion;
}

void FeatureQuads::GetVertices(float **vertices, int &num)
{
    num = m_vec2Vertices.size();
    (*vertices) = new float[num*2];
    for(int i=0;i<num;i++)
    {
        (*vertices)[2*i] = (float)(m_vec2Vertices[i].x);
        (*vertices)[2*i+1] = (float)(m_vec2Vertices[i].y);
    }
}

void FeatureQuads::GetIndices(int **indices, int &num)
{
    num = m_NumQuads;
    (*indices) = new int[m_NumQuads*4];
    for(int i=0;i<num;i++)
    {
        for(int j=0;j<4;j++)
            (*indices)[4*i+j] = m_quadConnectivity[4*i+j];
    }
}
