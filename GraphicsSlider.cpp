#include "GraphicsSlider.h"

#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

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
    // we need to consider the shadow around the handle
    m_handleSize = QSize( m_handlePixmap.width() - 4, m_handlePixmap.height() );
}

void GraphicsSlider::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // background
    painter->fillRect( boundingRect(), Qt::gray );

    // draw rail
    painter->setPen( QPen(Qt::white, 1) );
    painter->setBrush( Qt::cyan );
    painter->drawRect( m_railRect );

    // draw handler
    qreal valueRange = m_max - m_min;
    qreal handlePosRange = m_railRect.width() - m_handleSize.width();
    qreal handleCenterX = m_railRect.left() + m_handleSize.width() / 2 + handlePosRange * (m_value - m_min) / valueRange;

    qreal handleX = handleCenterX - m_handlePixmap.width() / 2;
    qreal handleY = boundingRect().center().y() - m_handlePixmap.height() / 2;

    painter->drawPixmap( handleX, handleY, m_handlePixmap );

    // draw border
    if ( hasFocus() )
    {
        qreal adjustment = BORDER_WIDTH / 2;
        QRectF borderRect = boundingRect().adjusted( adjustment, adjustment, -adjustment, -adjustment );
        painter->setPen( QPen(Qt::green, BORDER_WIDTH) );
        painter->drawLine( borderRect.bottomLeft(), borderRect.topLeft() );
        painter->drawLine( borderRect.topLeft(), borderRect.topRight() );
        painter->drawLine( borderRect.topRight(), borderRect.bottomRight() );
        painter->drawLine( borderRect.bottomRight(), borderRect.bottomLeft() );
    }
}

void GraphicsSlider::setValue(qreal value)
{
    m_value = qBound(m_min, value, m_max);
    update();
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

void GraphicsSlider::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    setFocus();

    m_isMousePressed = true;
    qreal x = event->pos().x();
    setHandlePos( x );
    event->accept();
}

void GraphicsSlider::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if ( ! m_isMousePressed )
    {
        return;
    }
    setHandlePos( event->pos().x() );
    event->accept();
}

void GraphicsSlider::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if ( ! m_isMousePressed )
    {
        return;
    }
    setHandlePos( event->pos().x() );
    m_isMousePressed = false;
    event->accept();
}

void GraphicsSlider::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "key pressed";
}

void GraphicsSlider::setHandlePos(qreal posX)
{
    qreal handleWidth = m_handleSize.width();
    qreal posRange = m_railRect.width() - handleWidth;
    qreal relativePos = posX - m_railRect.left() - handleWidth / 2;
    qreal value = relativePos * (m_max - m_min) / posRange + m_min;

    setValue( value );
}
