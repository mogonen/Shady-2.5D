#ifndef QSLIDERSPINBOX_H
#define QSLIDERSPINBOX_H

#include <QDoubleSpinBox>
#include <QSlider>
#include <QFrame>
#include <QMouseEvent>
#include <QtCore>

class QSliderSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit QSliderSpinBox();
    double m_Ratio;
    QFrame *m_SliderFrame;
    QSlider *m_Slider;
    double GetDoubleValue();

private:
protected:

signals:
    void SliderSpinboxValueChange(double);
public slots:
    void SetChangeRange(int low, int high);
    void SetChangeStep(double step);
    void SetDoubleValue(int value);
    void SetRatio(double ratio){m_Ratio = ratio;}
    void SetSliderValue(double value);
//    void PopSlider(double value);
};

#endif // QSliderSpinBox_H
