#include <QColorDialog>
#include "commands.h"
#include "canvas.h"

QColor              SetColor::COLOR;
SetColor::Source    SetColor::SOURCE = SetColor::TOOL;
bool                SetColor::EXEC_ONCLICK = true;
int                 SetColor::SELECT_MODE = 1;

Drag::DragTool     Drag::TOOL = Drag::NONE;
Drag::Continuity   Drag::CONT = Drag::C1;

Drag::Drag(Draggable_p pObj, const Vec2& t){
    _pObj = pObj;
    _t = t;
}

Drag::Drag(SelectionSet set, const Vec2 &t){
    _t = t;
    _selection  = set;
    _pObj = 0;
}

Command_p Drag::exec(){
    return 0;
}

Command_p Drag::unexec(){
    if (_pObj)
        _pObj->drag(-_t);

    if (!_selection.empty()){
        FOR_ALL_ITEMS(SelectionSet, _selection){
            if (!(*it)->isDraggable())
                continue;
            Draggable_p pDragged = (Draggable_p)(*it);
            pDragged->drag(-_t);
        }
    }
    return 0;
}

ShapeOrder::ShapeOrder(Operation op){
    _operation = op;
    _pShape = 0;
}

Command_p ShapeOrder::exec()
{
    _shapes = Session::get()->canvas()->shapes();
    if (_pShape==0)
        _pShape = Session::get()->theShape();

    if (_pShape == 0)
        return 0;

    switch(_operation)
    {
        case MOVE_UP:
            Session::get()->moveActiveUp();
            break;

        case MOVE_DOWN:
            Session::get()->moveActiveDown();
            break;

        case SEND_BACK:
            Session::get()->sendActiveBack();
            break;

        case SEND_FRONT:
            Session::get()->sendActiveFront();
            break;

        case INSERT_SHAPE:
            Session::get()->insertShape(_pShape);
        break;

        case DELETE_SHAPE:
            Session::get()->removeShape(_pShape);
        break;
    }
    return 0;
}

Command_p ShapeOrder::unexec(){        
    Session::get()->canvas()->setShapeList(_shapes);
    return 0;
}

//this shoud prefebly on select
void SetColor::onClick(const Click& click)
{
    if (!EXEC_ONCLICK)
        return;

    Channel channel = Session::channel();
    if (!(channel==DARK_CHANNEL || channel==BRIGHT_CHANNEL || channel==DEPTH_CHANNEL || channel==ALPHA_CHANNEL))
        return;

    if (!click.is(Click::UP))
        return;

   _pSB = dynamic_cast<ShapeBase_p>(Session::get()->selectionMan()->getLastSelected());

   if (_pSB)
       exec();
}

Command_p SetColor::exec()
{
    if (!_pSB && !Session::get()->selectionMan()->selectionSize())
        return new SetColor();

    _channel     = Session::channel();
    QColor color = COLOR;

    if (_pSB)
    {
        _col = _pSB->value[(int)_channel];
    }

    if (SOURCE == DIALOG)
    {
        RGBA col = _col*255;
        color = QColorDialog::getColor(QColor(col.x, col.y, col.z), (QWidget*)Session::get()->glWidget(), "Vertex Color",  QColorDialog::DontUseNativeDialog);
    }

    RGB rgb(color.redF(), color.greenF(), color.blueF());
    setColor(_pSB, rgb, SOURCE == BACKGROUND);
    SelectionSet selection = Session::get()->selectionMan()->getSelection();
    FOR_ALL_ITEMS(SelectionSet, selection)
    {
        ShapeBase_p sb = dynamic_cast<ShapeBase_p>(*it);
        setColor(sb, rgb);
    }

    return new SetColor();
}

void SetColor::setColor(ShapeBase_p pSB, const RGB &rgb, bool isbg)
{
    if (!pSB)
        return;

    if (isbg && Session::get()->canvas()->bgImage())
    {
        QColor col(Session::get()->canvas()->bgImage()->getColor(pSB->_P));
        //rgb order is flipped, don't know why...
        pSB->value[(int)_channel] = RGB(col.blueF(), col.greenF(),col.redF());
    }else
        pSB->value[(int)_channel] = rgb;

    pSB->outdate();
    if (selectMode() == SELECT_SHAPE)
    {
        Shape_p pShape = dynamic_cast<Shape_p>(pSB);
        if (!pShape)
            return;
        SVList svlist = pShape->getVertices();
        FOR_ALL_ITEMS(SVList, svlist)
                setColor(*it, rgb, isbg);
    }
}

Command_p SetColor::unexec()
{
    _pSB->value[_channel] = _col;
    return 0;
}

