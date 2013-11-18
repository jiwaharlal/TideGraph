#include "GraphicsSlider.h"

#include <QPainter>

#include <cassert>

const static int BORDER_WIDTH = 4;
const static int SLIDE_RAIL_WIDTH = 12;

GraphicsSlider::GraphicsSlider( qreal min, qreal max, QGraphicsItem* parent /* = NULL */ )
    : QGraphicsWidget( parent )
    , m_min( min )
    , m_max( max )
    , m_handlePixmap( ":/img/slider_handle" )
{
    assert( !m_handlePixmap.isNull() && "Slider handle pixmap is empty" );
}

void GraphicsSlider::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // background
//    painter->setBrush( Qt::gray );
    painter->fillRect( boundingRect(), Qt::gray );

    // draw rail
    painter->setPen( QPen(Qt::white, 1) );
    painter->setBrush( Qt::cyan );
    painter->drawRect( m_railRect );

    // draw handler
    qreal range = m_max - m_min;
    qreal handlerX = m_railRect.left() + ( m_railRect.width() - m_handlePixmap.width() ) * m_value / range;
    qreal handlerY = boundingRect().center().y() - m_handlePixmap.height() / 2;

    painter->drawPixmap( handlerX, handlerY, m_handlePixmap );

    // draw border
    qreal adjustment = BORDER_WIDTH / 2;
    QRectF borderRect = boundingRect().adjusted( adjustment, adjustment, -adjustment, -adjustment );
    painter->setPen( QPen(Qt::green, BORDER_WIDTH) );
    painter->drawLine( borderRect.bottomLeft(), borderRect.topLeft() );
    painter->drawLine( borderRect.topLeft(), borderRect.topRight() );
    painter->drawLine( borderRect.topRight(), borderRect.bottomRight() );
    painter->drawLine( borderRect.bottomRight(), borderRect.bottomLeft() );
}

void GraphicsSlider::setValue(qreal value)
{
    m_value = value;
}

void GraphicsSlider::setMinimum(qreal newMin)
{
    m_min = newMin;
}

void GraphicsSlider::setMaximum(qreal newMax)
{
    m_max = newMax;
}

void GraphicsSlider::resizeEvent(QGraphicsSceneResizeEvent *event)
{
    QRect r = boundingRect().toRect();
    int vMargin = ( r.height() - SLIDE_RAIL_WIDTH ) / 2;
    m_railRect = r.adjusted( BORDER_WIDTH, vMargin, -BORDER_WIDTH, -vMargin );
}

QSizeF GraphicsSlider::sizeHint(Qt::SizeHint which, const QSizeF &constraint) const
{
    QSizeF result = QGraphicsWidget::sizeHint( which, constraint );
    result.setHeight( m_handlePixmap.height() + BORDER_WIDTH + 2 );

    if ( which == Qt::MinimumSize )
    {
        result.setWidth( m_handlePixmap.width() * 2 );
    }

    return result;
}
