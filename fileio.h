#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include "base.h"

class MeshShape;
class EllipseShape;
class GridPattern;

struct Load{
    Selectable_p pObj;
    enum Key {PARENT, PAIR};
    int keys[2];
    Load(Selectable_p pO){
        pObj = pO;
    }
};

typedef std::map<int, Load*> LoadMap;

class DefaultIO:public FileIO
{
    std::ifstream            _infile;
    LoadMap                  _loadmap;

    bool write(Shape*, ofstream& outfile);
    bool read(Shape*);

    bool writeMeshShape(MeshShape*, ofstream&);
    bool parseMeshShape(MeshShape*);

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
