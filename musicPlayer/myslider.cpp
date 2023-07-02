
#include "myslider.h"

mySlider::mySlider(QWidget *parent):QSlider(parent)
{

}
mySlider::~mySlider()
{

}
void mySlider::mousePressEvent(QMouseEvent *event)
{
    int currentX = event->pos().x();

    double per = currentX *1.0 /this->width();

    //利用算得的百分比得到具体数字
    int value = per*(this->maximum() - this->minimum()) + this->minimum();

    //设定滑动条位置
    this->setValue(value);
    //滑动条移动事件等事件也用到了mousePressEvent,加这句话是为了不对其产生影响，使得Slider能正常响应其他鼠标事件
    QSlider::mousePressEvent(event);
}
