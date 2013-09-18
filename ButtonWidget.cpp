#include "ButtonWidget.h"

#include <QPainter>
#include <QDebug>

ButtonWidget::ButtonWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
{
    setMinimumSize(400, 200);
}

void ButtonWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(boundingRect(), Qt::green);
}

void ButtonWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "handling mouse press in ButtonWidget";
}
