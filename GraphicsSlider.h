#ifndef __GRAPHICS_SLIDER_H__
#define __GRAPHICS_SLIDER_H__

#include <QGraphicsWidget>

class GraphicsSlider: public QGraphicsWidget
{
public:
    GraphicsSlider( qreal min, qreal max, QGraphicsItem* parent = NULL );

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setValue( qreal value );
    void setMinimum( qreal newMin );
    void setMaximum( qreal newMax );
protected:
    virtual void resizeEvent(QGraphicsSceneResizeEvent *event) override;
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint) const;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void keyPressEvent(QKeyEvent *event);
private:
    void setHandlePos(qreal posX);

    qreal m_min;
    qreal m_max;
    qreal m_value;
    QPixmap m_handlePixmap;
    QRect m_railRect;
    QSize m_handleSize;
    bool m_isMousePressed;
};

#endif // __GRAPHICS_SLIDER_H__
