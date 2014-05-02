#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include <fstream>
#include "base.h"

class Shape;
class ShapeBase;
class MeshShape;
class EllipseShape;
class GridPattern;
class ImageShape;

struct Load{
    Draggable_p pObj;
    enum Key {PARENT, PAIR};
    int keys[2];
    Load(Draggable_p pO){
        pObj = pO;
    }
};

typedef std::map<int, Load*>            LoadMap;
typedef std::vector<std::string>        Tokens;

class DefaultIO:public FileIO
{
    Shape*                   _pShape;
    std::ifstream            _infile;
    LoadMap                  _loadmap;

    bool                    write(Shape*, ofstream& outfile);
    bool                    read(Shape*);

    bool                    writeMeshShape(MeshShape*, ofstream&);
    bool                    writeEllipseShape(EllipseShape*, ofstream&);
    bool                    writeImageShape(ImageShape * pIS, ofstream& outfile);

    bool                    parseMeshShape(MeshShape*);
    bool                    parseEllipseShape(EllipseShape*);
    bool                    parseImageShape(ImageShape*);

    void                    writeShapeBase(ShapeBase*, ofstream&);
    ShapeBase*              parseShapeBase(Tokens);

    Shape*                  parseShape(const char*);

public:

    bool                    load(const char *fname);
    bool                    save(const char *fname);
};

class INPExporter:public Exporter{

    //int*                  _nodePage;
   std::map<int, int>       _nodeMap;

    void**                  _edgeNodes;
    void**                  _vertexNodes;

    int                     nodeIndex(GridPattern*, int i, int j);
    int                     mapNodeId(GridPattern* patch, int i, int j, int node_id);

public:
    bool exportShape(Shape *, const char *fname);
};

class EPSExporter:public Exporter{

public:
    bool exportShape(Shape *, const char *fname);
};


#endif // FILEIO_H
