#include "LabelWidget.h"

#include <QPainter>

LabelWidget::LabelWidget(QGraphicsItem* parent)
    : QGraphicsWidget(parent)
{
    setMaximumSize(150, 40);
    setMinimumSize(150, 40);
}

LabelWidget::~LabelWidget()
{

}

void LabelWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(boundingRect(), Qt::red);
}
