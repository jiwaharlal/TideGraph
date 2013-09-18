#ifndef __LABEL_WIDGET_H__
#define __LABEL_WIDGET_H__

#include <QGraphicsWidget>

class LabelWidget: public QGraphicsWidget
{
public:
    LabelWidget(QGraphicsItem *parent = 0);
    virtual ~LabelWidget();
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // __LABEL_WIDGET_H__
