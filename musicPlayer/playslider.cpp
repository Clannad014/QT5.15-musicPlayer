
#include "playslider.h"

playslider::playslider(QWidget *parent):QSlider(parent)
{

}
playslider::~playslider()
{

}
void playslider::mousePressEvent(QMouseEvent *event)
{
    int currentX = event->pos().x();

    double per = currentX *1.0 /this->width();

    //利用算得的百分比得到具体数字
    int value = per*(this->maximum() - this->minimum()) + this->minimum();

    //设定滑动条位置
    this->setValue(value);
    emit mouseClick();
    //滑动条移动事件等事件也用到了mousePressEvent,加这句话是为了不对其产生影响，是的Slider能正常相应其他鼠标事件
    QSlider::mousePressEvent(event);
}

