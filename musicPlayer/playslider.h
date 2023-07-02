
#ifndef PLAYSLIDER_H
#define PLAYSLIDER_H


#include <QObject>
#include <QSlider>
#include <QMouseEvent>


class playslider :public QSlider
{
    Q_OBJECT


public:
    playslider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
    ~playslider();
signals:
    void mouseClick();
};

#endif // PLAYSLIDER_H
