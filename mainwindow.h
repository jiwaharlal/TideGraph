#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsWidget>

class TideGraphWidget;

class MainWindow : public QGraphicsWidget
{
    Q_OBJECT
    
public:
    MainWindow(QGraphicsScene& scene, QGraphicsWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;
private:
    TideGraphWidget* myTideGraph;
};

#endif // MAINWINDOW_H
