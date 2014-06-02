#ifndef PREVIEWATTRDIALOG_H
#define PREVIEWATTRDIALOG_H

#include "shape.h"
#include "../customdialog.h"

class PreviewAttrDialog: public CustomDialog
{
    Q_OBJECT

public:

    PreviewAttrDialog(Shape_p pShape, QString title, QWidget *parent = 0, char* execLabel = 0, void (*callback)()=0, bool * ischeck = 0);

    virtual void                Initialize();
    void                        SetNewSize(double w, double h);

private:

    Shape_p                     _pShape;

};

#endif // PREVIEWATTRDIALOG_H
