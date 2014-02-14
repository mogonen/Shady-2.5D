#ifndef FILEIO_H
#define FILEIO_H

#include <iostream>
#include "base.h"

class MeshShape;
class EllipseShape;

class DefaultIO:public FileIO
{

    bool write(Shape*, ofstream& outfile);
    bool read(Shape*, ifstream &);

    bool write(MeshShape*, ofstream&);
    bool read(MeshShape*,  ifstream&);

    Shape*  parseShape(const char*);

public:

    bool load(const char *fname);
    bool save(const char *fname);

    bool exportOBJ(Shape* pShape, char *fname);
    bool exportEPS(Shape* pShape, char *fname);

};


#endif // FILEIO_H
