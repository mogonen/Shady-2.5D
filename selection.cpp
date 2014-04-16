#include "base.h"
#include "commands.h"
#include <qDebug>
Selectable::Selectable(bool isUI):Renderable(isUI){
    _isDraggable = false;
    Session::get()->selectionMan()->insert(this);
}

Selectable::~Selectable(){
     Session::get()->selectionMan()->remove(this);
}

bool Selectable::isTheSelected() const {
    return this == Session::get()->selectionMan()->getTheSelected();
}

bool Selectable::isInSelection() const{
    return Session::get()->selectionMan()->isInSelection((Selectable_p)this);
}

int SelectionManager::INDEX = 0;

SelectionManager::SelectionManager(){
    _count = 0;
    _theSelected = 0;
    _lastSelected = 0;
}

void SelectionManager::insert(Selectable_p pS){
    if (pS==0) return;
    _count++;
    _selectables[_count] = pS;
    pS->_name = _count;
}

void SelectionManager::remove(Selectable_p pS){
    if (pS==0) return;
    _selectables.erase(pS->_name);
}

Selectable_p SelectionManager::get(int iname){
    SelectableMap::const_iterator it = _selectables.find(iname);
    if (it == _selectables.end())
        return 0;
    return it->second;
}

bool SelectionManager::isInSelection(Selectable_p pS){
    return _selection.find(pS) != _selection.end();
}


void SelectionManager::startSelect(Selectable_p pObj, const Click & click)
{
    isSelect = click.is(Click::DOWN);
    _theSelected = pObj;
    _click0 = click;
    _isdragged = false;

    if (pObj)
        pObj->onDown();
    else
        return;

    if (click.isCtrl){
        if (isSelect){
            pObj->_index = INDEX;
            _selection.insert(pObj);
            INDEX++;
        }else{
            _selection.erase(pObj);
            INDEX--;
             FOR_ALL_ITEMS(SelectionSet, _selection){
                 if ((*it)->_index > pObj->_index)
                     (*it)->_index--;
             }
        }
    }else if (_theSelected && !_theSelected->isInSelection())
        _selection.clear();
}

void SelectionManager::stopSelect(const Click& click){
    _lastSelected = _theSelected;
    _click1 = click;
    if (!_theSelected)
        return;

    if (_theSelected->isDraggable() && _isdragged){
        Vec2 t = _click1.P - _click0.P;

        if (!_selection.empty())
            Session::get()->exec(new Drag(_selection, t));
        else
            Session::get()->exec(new Drag((Draggable_p)_theSelected, t));
    }

    _theSelected->onUp();
    _theSelected = 0;
    _isdragged = false;
}

void SelectionManager::reset(){
    _selectables.clear();
    _count = 0;
}

void SelectionManager::cancelSelection(){
    _selection.clear();
    INDEX = 0;
}

bool SelectionManager::dragSelected(const Point& t, int button)
{

    if (!_theSelected || !_theSelected->isDraggable())
        return false;

    Draggable_p dragged = ((Draggable_p)_theSelected);
    if (dragged->isLocked)
        return false;

    if (!_selection.empty()){
        FOR_ALL_ITEMS(SelectionSet, _selection)
        {
            if (!(*it)->isDraggable())
                continue;
            Draggable_p pDragged = (Draggable_p)(*it);
            pDragged->drag(t, button);
        }
    }else
        dragged->drag(t, button);

    _isdragged = true;

    return true;
}

