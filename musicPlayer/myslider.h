
#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>

#include <QObject>
#include <QMouseEvent>


class mySlider : public QSlider
{
public:
    mySlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
    ~mySlider();
};

#endif // MYSLIDER_H
