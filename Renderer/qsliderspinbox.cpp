#include "qsliderspinbox.h"

QSliderSpinBox::QSliderSpinBox()
{
    m_SliderFrame = new QFrame( this, Qt::Popup);
    m_Slider = new QSlider(Qt::Horizontal, m_SliderFrame);
    connect(this, SIGNAL(valueChanged(double)), this, SLOT(SetSliderValue(double)));
    connect(m_Slider, SIGNAL(valueChanged(int)), this, SLOT(SetDoubleValue(int)));
}


void QSliderSpinBox::SetSliderValue(double value)
{
//    m_Slider->blockSignals(true);
    m_Slider->setValue(value*m_Ratio);
//    m_Slider->blockSignals(false);
    emit SliderSpinboxValueChange(value);
}

//void QSliderSpinBox::PopSlider(double value)
//{
////    if(m_SliderFrame->isHidden())
////    {
////        m_SliderFrame->move(this->mapToGlobal(this->pos()));
////        qDebug()<<this->mapToGlobal(this->pos());
////        m_SliderFrame->show();
////    }
//    SetSliderValue(value);
//}

void QSliderSpinBox::SetDoubleValue(int value)
{
    double double_value = (double)value/m_Ratio;
    blockSignals(true);
    setValue(double_value);
    blockSignals(false);
//    qDebug()<<this->value();
    emit SliderSpinboxValueChange(double_value);
}
void QSliderSpinBox::SetChangeRange(int low, int high)
{
    setRange((double)low/m_Ratio, (double)high/m_Ratio);
    m_Slider->setRange(low, high);
}

void QSliderSpinBox::SetChangeStep(double step)
{
    setSingleStep(step);
    m_Slider->setSingleStep(step*m_Ratio);
}

double QSliderSpinBox::GetDoubleValue()
{

    return (double)m_Slider->value()/m_Ratio;
}
