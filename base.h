#ifndef __AUXILIARY_H__
#define __AUXILIARY_H__
#include <vector>
#include <map>
#include <list>
#include <set>
#include "Vec/Matrix.h"


#define TYPE_BIT 24

#define FOR_ALL_ITEMS(ItemList, items) for(ItemList::iterator it = items.begin(); it != items.end(); it++)
#define FOR_ALL_ITEMS_2(ItemList, items) for(ItemList::iterator it2 = items.begin(); it2 != items.end(); it2++)
#define FOR_ALL_CONST_ITEMS(ItemList, items) for(ItemList::const_iterator it = items.begin(); it != items.end(); it++)
#define FOR_ALL_CONST_ITEMS_2(ItemList, items) for(ItemList::const_iterator it2 = items.begin(); it2 != items.end(); it2++)
#define FOR_ALL_I(size) for(int i =0; i < size; i++ )
#define FOR_ALL_J(size) for(int j =0; j < size; j++ )
#define CLAMP(n, min, max) ( (n) < (min) ? (min) : ( ( (n) > (max) ) ? (max) : (n) ) )

class Renderable;
class Selectable;
class Draggable;

class Shape;
class GLWidget;
class Canvas;
class ShapeControl;
class MainWindow;

enum RenderSetting {DRAGMODE_ON, SHADING_ON, AMBIENT_ON, SHADOWS_ON, NORMALS_ON, WIREFRAME_ON, PREVIEW_ON};
enum RenderMode {DEFAULT_MODE = 0, DRAG_MODE = 1, SM_MODE = 2, DARK_MODE = 4, BRIGHT_MODE = 8, LABELDEPTH_MODE = 16};


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

typedef std::list<Point> PointList;
typedef std::list<Point_p> Point_pList;

#define UI_BIT 31

typedef unsigned long RenderMode_t;

class Renderable{

protected:

    virtual void onOutdate(){}
    virtual void onUpdate(){}
    virtual void onEnsureUpToDate(){}
    static int mRenderMode;
public:
    virtual void render(int mode = 0) = 0;

    enum Type_e {NONE, UI, SHAPE};

    Renderable(Type_e type):_type(type){_upToDate = false;}
    Type_e type() const {return _type;}

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

    Void_p pRef; //generic pointer to refering object

private:
    Type_e _type;
    bool _upToDate;
};



class Selectable:public Renderable{

    int _name;
    bool _isDraggable;

    friend class SelectionManager;

protected:

    virtual void onUp(){}
    virtual void onDown(){}

public:

    enum Click_e{UP, DOWN, R_UP, R_DOWN};

    Selectable(Type_e type);
    virtual ~Selectable();

    virtual void render(int mode = 0);

//    void renderNamed(bool ispush = false) const;
//    void renderUnnamed() const{
//        render();
//    }

    inline bool isDraggable() const{return _isDraggable;}
    inline void makeDraggable(){_isDraggable = true;}

    int name() const{return _name;}
    bool isTheSelected() const;
    bool isInSelection() const;

};

class SelectionManager{

    int                 _count;
    Selectable_p        _theSelected;
    Selectable_p        _lastSelected;
    SelectableMap       _selectables;
    SelectionSet        _selection;

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

    void                startSelect(Selectable_p pObj, bool isselect, bool isMultiSelect);
    void                stopSelect();
    bool                dragTheSelected(const Vec2& t, int button = 0);
    void                reset();

    bool                isSelect;

};

class Draggable:public Selectable{

    Point_p _pP;

    DraggableList _childs;
    Draggable_p _parent;

protected:

    virtual void onDrag(const Point&, int button = 0){}

public:

    bool isLocked;

    Draggable(Type_e type, Point_p pP = 0):Selectable(type){
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

    inline Point    P() const{return *_pP;}
    inline Point_p  pP() const{return _pP;}

    void flipLock(){isLocked =!isLocked;}

    void drag(const Point& t, int button = 0){
        if(button == 0)
            _pP->set(*_pP + t);
        onDrag(t, button);
    }

    bool isChild() const {return _parent!=0;}
    bool isParent()const {return _childs.size()>0;}
    Draggable_p parent() const {return _parent;}

    bool adopt(Draggable_p pObj){
        if (pObj->_parent)
            return false;
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


class Session{

    GLWidget*           _pGlWidget;
    Canvas*             _pCanvas;
    SelectionManager*   _pSelectionMan;
    ShapeControl*       _pController;
    MainWindow*         _pMainWindow;

    static Session*     _pSession;

public:

    GLWidget*           glWidget() const {return _pGlWidget;}
    Canvas*             canvas()   const {return _pCanvas;}
    SelectionManager*   selectionMan()  const {return _pSelectionMan;}
    ShapeControl*       controller()    const {return _pController;}
    MainWindow*         mainWindow()    const {return _pMainWindow;}

    Shape*              theShape() const;
    void                activate(Shape*);
    Shape*              deactivate();
    void                insertShape(Shape*, bool isactivate = true);
    void                removeShape(Shape*);

    void                reset();

    void                cancel();
    void                exec();


    static void         init(MainWindow*);

    static Session*     get(){return _pSession;}
    static bool         isRender(RenderSetting rs);
};

class Renderer
{

public:
    virtual void render(RenderMode_t mode = 0) = 0;
};

struct ShapeVertex;
bool isInRenderMode();

#endif
