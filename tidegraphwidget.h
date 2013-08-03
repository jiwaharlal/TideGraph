#ifndef TIDEGRAPHWIDGET_H
#define TIDEGRAPHWIDGET_H

#include <QGraphicsWidget>

class TideGraphWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit TideGraphWidget(QGraphicsItem *parent = 0);
    
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;
signals:
    
public slots:
    
private:
    QPixmap myUpExtremePixmap;
    QPixmap myTabPixmap;
};

#endif // TIDEGRAPHWIDGET_H
