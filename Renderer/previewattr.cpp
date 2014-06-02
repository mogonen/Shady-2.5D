#include "PreviewAttrDialog.h"

PreviewAttrDialog::PreviewAttrDialog(Shape_p pShape, QString title, QWidget *parent, char* execLabel, void (*callback)(), bool * ischeck):CustomDialog(title, parent, execLabel,callback,ischeck)
{
    _pShape = pShape;
}

void PreviewAttrDialog::Initialize()
{
    this->addDblSpinBoxF("AlphaTh:", 0, 1, &_pShape->m_alpha_th, 2);
    //this->addDblSpinBoxF("Strech:", -10, 10, &_pShape->m_stretch, 1);
    this->addDblSpinBoxF("Depth:", 0, 1, &_pShape->m_assignedDepth, 2);
    this->addDblSpinBoxF("Refract:", -1, 1, &_pShape->_shaderParam.m_alphaValue, 2);
    this->addCheckBox("Shadow Creator:", &_pShape->_shaderParam.m_shadowcreator);
    this->addCheckBox("Toggled Reflection:", &_pShape->_shaderParam.m_reflectToggled);
}
