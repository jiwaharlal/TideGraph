#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <QGraphicsView>

class MainView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit MainView(QGraphicsScene *parent = 0);
    
protected:
//    void mousePressEvent(QMouseEvent *event);
    //void mouseReleaseEvent(QEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
signals:
    
public slots:
    
};

#endif // MAINVIEW_H
