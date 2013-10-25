#ifndef SAMPLESHAPE_H
#define SAMPLESHAPE_H

#include "shape.h"

class SampleShape: public Shape{


public:

    SampleShape():Shape(){
        Point p[4];
        p[0].set(-0.1, 0.1);
        p[1].set(0.1, 0.1);
        p[2].set(0.1, -0.1);
        p[3].set(-0.1, -0.1);

        for(int i=0; i<4; i++)
            addVertex(p[i]);
    }

    void render() const;
};

#endif // SAMOLESHAPE_H
