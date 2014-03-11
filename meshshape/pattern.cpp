#include "meshcommands.h"
#include "patch.h"

int         PatternOperation::FOLD_N           = 8;
double      PatternOperation::FOLD_W          = 0;
string      PatternOperation::PATTERN          = "1,2";


int * parsePattern(string patternstr , int &len)
{
    char * cstr = new char [patternstr.length()+1];
    std::strcpy (cstr, patternstr.c_str());

    int * pattern = new int[24];
    char * tok = strtok(cstr,",");
    len = 0;
    while (tok != NULL)
    {
       pattern[len++] = atoi(tok);
       tok = strtok (NULL, ",");
    }
    return pattern;
}

void  PatternOperation::assignPattern(Edge_p pE, string patternstr)
{
    if (!pE)
        return;

    Corner* c0 = pE->C0();

    int len;
    int * pattern = parsePattern(patternstr, len);

    int off = 0;
    do{
        if (c0->isBorder())
            break;

        PatternPatch* patch = dynamic_cast<PatternPatch*>(c0->F()->pData->pSurface);
        if (!patch)
            return;

        int i = c0->I();
        patch->assignPattern(i%2, off, len, pattern);
        off+= (i%2) ? patch->U() : patch->V();

        c0 = c0->prev()->vPrev();

    }while(c0 != pE->C0());
}

void PatternOperation::setFolds(Edge_p pE, int n, double w)
{
    if (!pE )
        return;

    //Mesh_p pMesh = pE->mesh();
    Corner* c0 = pE->C0();
    do{
        if (c0->isBorder())
            break;

        PatternPatch* patch = dynamic_cast<PatternPatch*>(c0->F()->pData->pSurface);
        if (!patch)
            return;
        int i = c0->I();
        if (i%2)
            patch->init(patch->U(), n);
        else
            patch->init(n, patch->V());

        c0 = c0->prev()->vPrev();

    }while(c0 != pE->C0());

}
