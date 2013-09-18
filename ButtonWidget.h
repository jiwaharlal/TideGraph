#ifndef __BUTTON_WIDGET_H__
#define __BUTTON_WIDGET_H__

#include <QGraphicsWidget>

class ButtonWidget: public QGraphicsWidget
{
public:
    ButtonWidget(QGraphicsItem* parent = NULL);
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // __BUTTON_WIDGET_H__
