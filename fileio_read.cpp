#include <string>
#include <sstream>

#include "fileio.h"
#include "shape.h"
#include "MeshShape/cmesh.h"
#include "MeshShape/meshshape.h"
#include "MeshShape/curvededge.h"
#include "ellipseshape.h"

#ifndef MODELING_MODE
#include "Renderer/imageshape.h"
#endif

struct SVLoad{
    ShapeVertex* sv;
    int parent_id, pair_id;
    SVLoad(ShapeVertex* sv_, int parent, int pair){
        sv = sv_;
        parent_id = parent;
        pair_id = pair;
    }
};

struct EdgeLoad
{
    Edge_p e;
    int f0, c0, f1, c1;
    EdgeLoad(Edge_p e_, int f0_, int c0_, int f1_, int c1_ )
    {
        e = e_;
        f0 = f0_;
        c0 = c0_;
        f1 = f1_;
        c1 = c1_;
    }
};

typedef std::map<int, SVLoad*>          SVLoadMap;
typedef std::map<int, Vertex_p>         VertexLoadMap;
typedef std::map<int, Edge_p>           EdgeLoadMap;
typedef std::map<int, Face_p>           FaceLoadMap;


void split(const std::string &s, char delim, Tokens &elems)
{
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty())
            elems.push_back(item);
    }
}

bool DefaultIO::load(const char *fname)
{
    _infile.open (fname);
    std::string readline;
    bool isok = true;

    _pShape = 0;
    ShapeList shapes;

    while(std::getline(_infile, readline))
    {
        if (readline.empty())
            continue;

        Tokens toks;
        split(readline, ' ', toks);
        std::string label = toks[0];

        if (label.compare("s") == 0)
        {
            ShapeBase_p pSB = parseShapeBase(toks);
            if (pSB->type() != SHAPE_VERTEX){
                _pShape = (Shape_p)pSB;
                shapes.push_back(_pShape);
            }
        }else if (_pShape && label.compare("#shapedata") == 0){
            switch(_pShape->type())
            {
            case MESH_SHAPE:
                parseMeshShape((MeshShape*)_pShape);
                break;

            case ELLIPSE_SHAPE:
                 parseEllipseShape((EllipseShape*)_pShape);
                break;

            case IMAGE_SHAPE:
                 parseImageShape((ImageShape*)_pShape);
                break;
            }
        }
    }

    _infile.close();
    FOR_ALL_ITEMS(LoadMap, _loadmap)
    {
        //int id = it->first;
        Load* load = it->second;
        if (load->keys[Load::PARENT])
        {
            /*int pk = load->keys[Load::PARENT];
            ShapeBase_p pSBParent = _loadmap[pk]->pObj;
            Draggable_p parent = ((Draggable_p)pSBParent);
            parent->adopt((Draggable_p)load->pObj);*/
           ShapeVertex_p parent = ((ShapeVertex_p)(_loadmap[load->keys[Load::PARENT]]->pObj));
           parent->adopt((Draggable_p)load->pObj);
        }

        if (load->keys[Load::PAIR])
        {
            ShapeVertex_p pair = ((ShapeVertex_p)(_loadmap[load->keys[Load::PAIR]]->pObj));
            pair->setPair((ShapeVertex_p)load->pObj);
        }
    }

    FOR_ALL_ITEMS(ShapeList, shapes)
    {
        Shape* pShape = (*it);
        pShape->update();
        Session::get()->insertShape(pShape);
    }
    return isok;
}

ShapeBase* DefaultIO::parseShapeBase(Tokens toks){

    int type = atoi(toks[1].c_str());
    ShapeBase_p pSB = 0;
    Load* load = 0;
    switch(type){

    case SHAPE_VERTEX:{
        ShapeVertex_p sv = _pShape->addVertex();
        load = new Load(sv);
        pSB = sv;
    }
        break;

    case MESH_SHAPE:{
        Shape_p pMS = new MeshShape();
        load = new Load(pMS);
        pSB = pMS;
    }
        break;

    case ELLIPSE_SHAPE:{
        Shape_p pES = new EllipseShape();
        load = new Load(pES);
        pSB = pES;
    }
        break;

#ifndef MODELING_MODE
    case IMAGE_SHAPE:{
        Shape_p pIS = new ImageShape();
        load = new Load(pIS);
        pSB = pIS;
    }
        break;
#endif
    }

    int name, parent, pair;
    sscanf(toks[2].c_str(),"%d/%d/%d", &name, &parent, &pair);
    //Load* load = new Load((Draggable_p)pSB);
    if (load){
        load->keys[Load::PARENT] = parent;
        load->keys[Load::PAIR] = pair;
        _loadmap[name] = load;
    }

    float px, py;
    sscanf(toks[3].c_str(),"%f", &px);
    sscanf(toks[4].c_str(),"%f", &py);
    pSB->_P.set(px, py);

    for(int c = 0; c < ACTIVE_CHANNELS; c++)
    {
        float vx, vy, vz;
        sscanf(toks[5 + c*3].c_str(),"%f", &vx);
        sscanf(toks[6 + c*3].c_str(),"%f", &vy);
        sscanf(toks[7 + c*3].c_str(),"%f", &vz);
        pSB->value[c].set(vx, vy, vz);
    }

    return pSB;
}

bool DefaultIO::parseMeshShape(MeshShape * pMS)
{

    Mesh_p pMesh = pMS->mesh();

    std::string line;
    VertexLoadMap vertmap;
    FaceLoadMap facemap;

    while(std::getline(_infile, line))
    {
        if (line.empty())
            break;

        Tokens toks;
        split(line, ' ', toks);
        std::string label = toks[0];

        if (label.compare("surface")==0){
            int patch;
            sscanf(toks[1].c_str(),"%d", &patch);
            pMS->_patchtype = (Patch::PatchType) patch;
        }

        if (label.compare("f")==0)
        {
            int size = toks.size()-3;
            Face_p pF = pMesh->addFace(size, toks[size+2].compare("1")==0 );

            int fid;
            sscanf(toks[1].c_str(),"%d", &fid);
            facemap[fid] = pF;
            for(int i=0; i < size; i++){
                int svid;
                sscanf(toks[i+2].c_str(),"%d", &svid);
                ShapeVertex_p sv = (ShapeVertex_p)_loadmap[svid]->pObj;
                VertexLoadMap::iterator it = vertmap.find(svid);
                Vertex_p pV = 0;
                if (it == vertmap.end()){//create new vertex
                    pV = pMS->addMeshVertex(sv);
                    vertmap[svid] = pV;
                }else
                    pV = it->second;

                pV->set(pF->C(i));
            }
            pF->update();
        }
        else if (label.compare("e")==0)
        {
            int c0f, c0i,c1f, c1i, sv1, sv2;
            sscanf(toks[1].c_str(), "%d/%d", &c0f, &c0i);
            sscanf(toks[2].c_str(), "%d/%d", &c1f, &c1i);
            sscanf(toks[3].c_str(), "%d", &sv1);
            sscanf(toks[4].c_str(), "%d", &sv2);

            Edge_p pE = pMesh->addEdge(0);
            pE->set(facemap[c0f]->C(c0i), 0);
            pE->set(facemap[c1f]->C(c1i), 1);
            pE->pData = new CurvedEdge(pE, 0);

            if (sv1 && sv2)
                pE->pData->init((ShapeVertex_p)_loadmap[sv1]->pObj, (ShapeVertex_p)_loadmap[sv2]->pObj);
            else
                pE->pData->init(0,0);
        }
    }
}

bool DefaultIO::parseEllipseShape(EllipseShape* pShape)
{
    std::string line;
    std::getline(_infile, line);
    Tokens toks;
    split(line, ' ', toks);
    if (toks[0].compare("rad"))
    {
        float rx, ry;
        sscanf(toks[1].c_str(), "%f", &rx);
        sscanf(toks[2].c_str(), "%f", &ry);
        pShape->setRad(rx, ry);
    }
}

bool DefaultIO::parseImageShape(ImageShape* pShape)
{
#ifndef MODELING_MODE

    /*legend:
     *
        double                      m_alpha_th;
        double                      m_stretch;
        double                      m_assignedDepth;
        double                      m_width;
        double                      m_height;

        int                         m_texUpdate;
        bool                        m_shadowCreator;
        GLuint                      m_texSM;
        GLuint                      m_texDark;
        GLuint                      m_texBright;
        GLuint                      m_texDisp;

        QString                     m_SMFile;
        QString                     m_DarkFile;
        QString                     m_BrightFile;
        QString                     m_DispFile;
    */

    float alpha, strech, depth, width, height;
    std::string line;
    std::getline(_infile, line);
    sscanf(line.c_str(), "%f, %f, %f, %f, %f", &alpha, &strech, &depth, &width, &height);
    pShape->m_alpha_th = alpha;
    pShape->m_stretch = strech;
    pShape->m_assignedDepth = depth;
    pShape->m_width = width;
    pShape->m_height = height;

    int text, shadow, sm, dark, bright, disp;
    std::getline(_infile, line);
    sscanf(line.c_str(), "%d, %d, %d, %d, %d", &text, &shadow, &sm, &dark, &bright, &disp);
    //pShape->m_texUpdate         = text;
    pShape->m_shadowCreator     = shadow;
    pShape->m_texSM             = sm;
    pShape->m_texDark           = dark;
    pShape->m_texBright         = bright;
    pShape->m_texDisp           = disp;

    int channel = 0;
    while(std::getline(_infile, line)){
        if (line.empty())
            break;
        pShape->m_fileName[channel] =  QString::fromStdString(line);
        pShape->m_texUpdate = pShape->m_texUpdate  | (1 << channel);
        channel++;
    }

#endif
}
