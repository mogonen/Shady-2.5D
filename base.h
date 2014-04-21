#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__
#include <vector>
#include <map>
#include <list>
#include <set>
#include "Vec/Matrix.h"
#include "ref.h"


#define TYPE_BIT 24

#define FOR_ALL_ITEMS(ItemList, items) for(ItemList::iterator it = items.begin(); it != items.end(); it++)
#define FOR_ALL_ITEMS_2(ItemList, items) for(ItemList::iterator it2 = items.begin(); it2 != items.end(); it2++)
#define FOR_ALL_CONST_ITEMS(ItemList, items) for(ItemList::const_iterator it = items.begin(); it != items.end(); it++)
#define FOR_ALL_CONST_ITEMS_2(ItemList, items) for(ItemList::const_iterator it2 = items.begin(); it2 != items.end(); it2++)
#define SKIP_DELETED(pItem) if (pItem->isDeleted()) continue;
#define SKIP_DELETED_ITEM if ((*it)->isDeleted()) continue;
#define FOR_ALL_I(size) for(int i =0; i < size; i++ )
#define FOR_ALL_J(size) for(int j =0; j < size; j++ )
#define CLAMP(n, min, max) ( (n) < (min) ? (min) : ( ( (n) > (max) ) ? (max) : (n) ) )
#define MAX(x,y) ( (x) > (y) ? (x) : (y))
#define MIN(x,y) ( (x) < (y) ? (x) : (y))

class Renderable;
class Selectable;
class Draggable;

class Shape;
class GLWidget;
class Canvas;
class ShapeControl;
class MainWindow;

enum RenderSetting  {DRAG_ON, SHADING_ON, AMBIENT_ON, SHADOWS_ON, NORMALS_ON, WIREFRAME_ON, CURVES_ON, SURFACES_ON, PREVIEW_ON, SHOW_ISOLATED, BACKGROUND_ON};
enum PreviewSetting {DEFAULT_MODE = 0, DRAG_MODE = 1, SM_MODE = 2, DARK_MODE = 4, BRIGHT_MODE = 8, LABELDEPTH_MODE = 16, DISPLACE_MODE = 32};
        //{DEFAULT_MODE = 0, SM_MODE = 1, DARK_MODE = 2, BRIGHT_MODE = 4, LABELDEPTH_MODE = 8};


typedef void* Void_p;
typedef Renderable* Renderable_p;
typedef Selectable* Selectable_p;
typedef Draggable* Draggable_p;
typedef std::map<int, Selectable_p> SelectableMap;
typedef std::list<Draggable_p> DraggableList;
typedef std::set<Selectable_p> SelectionSet;

typedef Vec2    Point;
typedef Vec2*   Point_p;

typedef Vec3    Normal;
typedef Vec3*   Normal_p;

typedef Vec4    RGBA;
typedef Vec4*   RGBA_p;


typedef std::list<Point> PointList;
typedef std::list<Point_p> Point_pList;

#define UI_BIT 31

class Renderable{

protected:

    virtual void onOutdate(){}
    virtual void onUpdate(){}
    virtual void onEnsureUpToDate(){}
    static int mRenderMode;
public:
    virtual void render(int mode = 0) = 0;


    Renderable(bool isUI):_isUI(isUI){_upToDate = false;}
    bool isUI() const {return _isUI;}

//    void renderUpToDate() {
//        ensureUpToDate();
////        render();
//    }

    void outdate(){
        _upToDate = false;
        onOutdate();
    }

    void ensureUpToDate(){
        if (!_upToDate)
            update();
        onEnsureUpToDate();
    }

    void update(){
        onUpdate();
        _upToDate = true;
    }

private:
    bool _isUI;
    bool _upToDate;
};


struct Click
{
    enum Type {NONE, UP, DOWN, R_UP, R_DOWN};

    Point       P;
    Type        type;
    bool        isCtrl, isAlt;

    inline bool is(Type t) const {return type == t;}
    Click(){type=NONE; isCtrl = false; isAlt = false;}
    Click(Type t, const Point& p){
        type = t;
        P    = p;
    }

};

class Selectable:public Renderable, public Referrer{

    int             _name;
    bool            _isDraggable;
    friend class    SelectionManager;

    int             _index;

protected:

    virtual void    onUp(){}
    virtual void    onDown(){}

    inline void     makeDraggable(){ _isDraggable = true; }

public:

    Selectable(bool isUI);
    virtual ~Selectable();

    virtual void    render(int mode = 0);

    inline bool     isDraggable() const{return _isDraggable;}

    int             name() const{return _name;}
    bool            isTheSelected() const;
    bool            isInSelection() const;


    int                 I()const {return _index;}

};

class SelectionManager{

    int                 _count;
    Selectable_p        _theSelected;
    Selectable_p        _lastSelected;
    SelectableMap       _selectables;
    SelectionSet        _selection;

    bool                _isdragged;

    Click               _click0, _click1;

public:
    SelectionManager();

    void                insert(Selectable_p pS);
    void                remove(Selectable_p pS);
    Selectable_p        get(int iname);
    Selectable_p        getTheSelected(){return _theSelected;}
    Selectable_p        getLastSelected(){return _lastSelected;}
    SelectionSet        getSelection(){return _selection;}
    int                 selectionSize(){return _selection.size();}

    bool                isInSelection(Selectable_p pS);

    void                startSelect(Selectable_p pObj, const Click&);
    void                stopSelect(const Click&);
    bool                dragSelected(const Vec2& t, int button = 0);
    void                reset();
    void                cancelSelection();

    bool                isSelect;

    static int          INDEX;

};

class Draggable:public Selectable{

    Point_p             _pP;

    DraggableList       _childs;
    Draggable_p         _parent;


protected:

    virtual void onDrag(const Point& t, int button = 0)
    {
        if (isParent())
        {
            FOR_ALL_CONST_ITEMS(DraggableList, _childs){
                if ((*it)->pP())
                    (*it)->pP()->set((*it)->P() + t);
            }
        }
    }

public:

    bool isLocked;

    Draggable(bool isUI, Point_p pP = 0):Selectable(isUI)
    {
        if (pP){
            makeDraggable();
        }
        _pP = pP;
        _parent = 0;
        isLocked = false;
    }

   virtual ~Draggable(){
        unparent();
        for(list<Draggable_p>::iterator it = _childs.begin(); it!= _childs.end(); it++)
            (*it)->_parent = 0;
    }

    virtual inline Point P()    const{return *_pP;}
    inline Point_p  pP()        const{return _pP;}

    void flipLock(){isLocked =!isLocked;}

    void drag(const Point& t, int button = 0){
        if(button == 0 && _pP){
            _pP->set(*_pP + t);
        }
        onDrag(t, button);
    }

    bool isChild() const {return _parent!=0;}
    bool isParent()const {return _childs.size()>0;}
    Draggable_p parent() const {return _parent;}

    bool adopt(Draggable_p pObj, bool isunparent=true){
        if (pObj->_parent){
            if (isunparent)
                pObj->unparent();
            else
                return false;
        }
        _childs.push_back(pObj);
        pObj->_parent = this;
        return true;
    }

    void unparent(){
        if (_parent)
            _parent->_childs.remove(this);
        _parent = 0;
    }

    //statics

    DraggableList getChilds() const {return _childs;}
};

struct ShapeVertex;
bool isInRenderMode();


enum SelectionMode {NOSELECT, SELECT_SHAPE, SELECT_VERTEX, SELECT_TANGENT, SELECT_VERTEX_TANGENT, SELECT_VERTEX_VERTEX, SELECT_EDGE, SELECT_FACE, SELECT_CORNER, SELECT_EDGE_EDGE};
enum Channel       {NORMAL_CHANNEL, DARK_CHANNEL, BRIGHT_CHANNEL, DEPTH_CHANNEL, ALPHA_CHANNEL, DISP_CHANNEL, GL_SHADING, PREVIEW};

#define ACTIVE_CHANNELS 4

typedef class Command{
    int _id;

protected:

    virtual void onSelect(){}
    virtual void onUnselect(){}
    virtual void onCancel(){}
    virtual void onClick(const Click&){}

public:

    enum CommandType        {NONE, MESH_OPERATION, MESH_PRIMITIVE, DRAG, SHAPE_ORDER, SET_COLOR};//needed?

    //enum ExecMode    {NONE, ON_SELECT, ON_UP, ON_DOWN, ON_};

    virtual Command*        exec() = 0;
    virtual Command*        unexec() = 0;
    virtual CommandType     type() const = 0;

    void                    select(){onSelect();}
    void                    unselect(){onUnselect();}
    void                    cancel(){onCancel();}
    void                    sendClick(const Click& c){onClick(c);}

    virtual SelectionMode   selectMode() const {return NOSELECT;}

}* Command_p;

typedef list<Command_p> CommandList;

class FileIO {

public:

    virtual bool load(const char * fname) = 0;
    virtual bool save(const char * fname) = 0;
};

class Exporter {

public:
    virtual bool exportShape(Shape*, const char * fname) = 0;
};


class Session{

    GLWidget*           _pGlWidget;
    Canvas*             _pCanvas;
    SelectionManager*   _pSelectionMan;
    ShapeControl*       _pController;
    MainWindow*         _pMainWindow;
    FileIO*             _pFileIO; //default;
    Exporter*           _pExporters[10];

    const char*         _filename;

    CommandList         _commands;
    Command_p           _pTheCommand;
    Shape*              _pTheShape;
    Channel             _channel;


    static Session*     _pSession;

public:

    GLWidget*           glWidget()      const {return _pGlWidget;}
    Canvas*             canvas()        const {return _pCanvas;}
    SelectionManager*   selectionMan()  const {return _pSelectionMan;}
    ShapeControl*       controller()    const {return _pController;}
    MainWindow*         mainWindow()    const {return _pMainWindow;}
    FileIO*             fileIO()        const {return _pFileIO;}
    Command_p           theCommand()    const {return _pTheCommand;}
    Shape*              theShape()      const {return _pTheShape;}

    void                activate(Shape*);
    Shape*              deactivate();
    void                insertShape(Shape*, bool isactivate=true);
    void                removeShape(Shape*);
    void                removeActiveShape();
    void                moveActiveDown();
    void                moveActiveUp();
    void                sendActiveBack();
    void                sendActiveFront();
    void                selectNext();
    void                selectPrev();

    static void         init(MainWindow*);

    static Session*     get(){return _pSession;}
    static bool         isRender(RenderSetting rs);
    static Channel      channel();
    static SelectionMode selectMode();

    void                open(const char *fname);
    void                saveAs(const char* fname);
    void                exportShape(const char *fname, int exporterid);
    int                 save();
    void                reset();
    void                setBG(const char *fname);

    //Manage Commands
    void                setCommand(Command_p);
    void                exec(Command_p pCommand = 0);
    int                 undo();
    int                 redo();
    void                cancel();

    void                sendClick(const Click&);

    void                setChannel(Channel channel);


};

#endif
