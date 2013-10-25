#ifndef FACIALSHAPE_H
#define FACIALSHAPE_H

#include "symmetryquad.h"

#include "meshshape/meshshape.h"

class FacialShape : public MeshShape
{
public:
    FacialShape();
    std::vector<std::vector<Point *> > CvtGrid(const std::vector<cv::Point2d> &vertices, const std::vector<std::vector<int> >  &grid_ind, float w, float h);
    std::vector<std::vector<Normal *> > CvtGrid(const std::vector<cv::Vec3f> &colors, const std::vector<std::vector<int> >  &grid_ind);
    void render() const;
    void initBG();
    std::string m_imgName;
};

#endif // FACIALSHAPE_H
