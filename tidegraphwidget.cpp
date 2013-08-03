#include "tidegraphwidget.h"

#include <QPainter>

TideGraphWidget::TideGraphWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , myUpExtremePixmap(":/img/up_extreme")
    , myTabPixmap(":/img/tab")
{
    setPos(0, 0);
    setMaximumSize(300, 300);
    setMinimumSize(300, 300);
}

void TideGraphWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->fillRect(QRectF(QPointF(0, 0), size()), QColor(150, 150, 159));
    painter->drawPixmap(0, 0, myUpExtremePixmap);
    QPoint tabPos(50, 50);
    painter->drawPixmap(tabPos, myTabPixmap);
    QRectF textRect(QPointF(tabPos), myTabPixmap.size());
    QFont font = painter->font();
    font.setPixelSize(28);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignCenter, "42");
}
