#include "fileio.h"
#include "meshshape/pattern.h"
#include "meshshape/meshshape.h"
#include "EPSPlot.h"

bool EPSExporter::exportShape(Shape * shape, const char *fname){


    EPSPlot plot(fname, 35, 0, 202, 101);

    MeshShape* pMS = dynamic_cast<MeshShape*>(shape);

    if (!pMS)
        return false;

    EdgeList edges = pMS->mesh()->edges();
    FOR_ALL_ITEMS(EdgeList, edges){
        SKIP_DELETED_ITEM;

        Edge_p pE = (*it);

        if (!pE->isBorder())
            continue;

        int size = pE->pData->size();
        float px[50];
        float py[50];
        FOR_ALL_I(size){
            Point p = pE->pData->P(i);
            px[i] = p.x;
            py[i] = p.y;
        }
        plot.drawLines(px, py,size);
    }


    FaceList faces = pMS->mesh()->faces();
    FOR_ALL_ITEMS(FaceList, faces){
        SKIP_DELETED_ITEM;

        Face_p pF = (*it);
        UVPatternPatch* uvpatch = dynamic_cast<UVPatternPatch*>(pF->pData);

        if (!uvpatch)
            continue;

        FOR_ALL_J(2){

            FOR_ALL_I(uvpatch->UV(j)){

                int pattern = uvpatch->getPattern(i, j); // 0, 1, 2 (none, red, green etc)

                int size = (j==0)? uvpatch->USamples() : uvpatch->VSamples();
                float px[50];
                float py[50];

                for(int k=0; k < size; k++){
                    Point p = uvpatch->P(j, i, k);
                    px[k] = p.x;
                    py[k] = p.y;
                }
                plot.drawLines(px, py,size);
            }
        }
    }

}
