#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include "base.h"

class MeshShape;
class EllipseShape;
class GridPattern;

struct SVLoad{
    ShapeVertex* sv;
    int parent_id, pair_id;
    SVLoad(ShapeVertex* sv_, int parent, int pair){
        sv = sv_;
        parent_id = parent;
        pair_id = pair;
    }
};

typedef std::map<int, SVLoad*> SVLoadMap;

class DefaultIO:public FileIO
{

    bool write(Shape*, ofstream& outfile);
    bool read(Shape*, ifstream &);

    bool write(MeshShape*, ofstream&);
    bool read(MeshShape*,  ifstream&, SVLoadMap &svmap);

    Shape*  parseShape(const char*);

public:

    bool load(const char *fname);
    bool save(const char *fname);
};


class INPExporter:public Exporter{

    //int*        _nodePage;

   std::map<int, int> _nodeMap;

    void**      _edgeNodes;
    void**      _vertexNodes;

    int nodeIndex(GridPattern*, int i, int j);
    int mapNodeId(GridPattern* patch, int i, int j, int node_id);

public:
    bool exportShape(Shape *, const char *fname);
};

#endif // FILEIO_H
