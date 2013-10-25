#include "facialshape.h"
#include "Canvas.h"

FacialShape::FacialShape()
{
    SymmetryQuad mSymQuad;
    mSymQuad.AssignNormalColor();
    m_imgName = mSymQuad.GetFileName();
    vector<vector<int> > gridInd = mSymQuad.CreatGridIndices();
    std::vector<std::vector<Point *> > PointsGrid = CvtGrid(mSymQuad.GetVerticesVector(), gridInd, (float)mSymQuad.m_width, (float)mSymQuad.m_height);
    std::vector<std::vector<Normal *> > ColorGrid = CvtGrid(mSymQuad.GetColorVector(), gridInd);
    std::vector<std::vector<Vertex *> > VerticesGrid;

    //insert quad faces only
    _control = new Mesh();
    _control->setInsertEdgeCB(onInsertEdge);
    _control->setAddFaceCB(onAddFace);
    _control->setCaller((void*)this);

    VerticesGrid.resize(PointsGrid.size());
    for(int i=0;i<PointsGrid.size();i++)
    {
        VerticesGrid[i].resize(PointsGrid[i].size());
        for(int j=0;j<PointsGrid[i].size();j++)
        {
            VerticesGrid[i][j] = addMeshVertex((*PointsGrid[i][j]));
        }
    }

    for(int i=0;i<VerticesGrid.size()-1;i++)
    {
        for(int j=0;j<VerticesGrid[i].size()-1;j++)
            _control->addQuad(VerticesGrid[i][j], VerticesGrid[i][j+1], VerticesGrid[i+1][j+1], VerticesGrid[i+1][j]);
    }

    MeshShape::isSMOOTH = false;
    _control->buildEdges();

    makeSmoothTangents();
    initBG();

    Renderable::update();

}

std::vector<std::vector<Point *> > FacialShape::CvtGrid(const std::vector<cv::Point2d> &vertices, const std::vector<std::vector<int> > &grid_ind, float w, float h)
{
    std::vector<std::vector<Point *> > VGrid;
    VGrid.resize(grid_ind.size());
    for(int i=0;i<grid_ind.size();i++)
    {
        VGrid[i].resize(grid_ind[i].size());
        for(int j=0;j<grid_ind[i].size();j++)
            VGrid[i][j] = new Point(vertices[grid_ind[i][j]].x/w*2-1,-vertices[grid_ind[i][j]].y/h*2+1);
    }

    return VGrid;
}

std::vector<std::vector<Normal *> > FacialShape::CvtGrid(const std::vector<cv::Vec3f> &colors, const std::vector<std::vector<int> > &grid_ind)
{
    std::vector<std::vector<Normal *> > CGrid;
    CGrid.resize(grid_ind.size());
    for(int i=0;i<grid_ind.size();i++)
    {
        CGrid[i].resize(grid_ind[i].size());
        for(int j=0;j<grid_ind[i].size();j++)
        {
            CGrid[i][j] = new Normal(2*colors[grid_ind[i][j]][0]-1,2*colors[grid_ind[i][j]][1]-1,2*colors[grid_ind[i][j]][2]-1);
            CGrid[i][j]->normalize();
        }
    }

    return CGrid;

}
