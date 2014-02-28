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
};


class INPExporter:public Exporter{

public:
    bool exportShape(Shape *, const char *fname);
};

#endif // FILEIO_H
