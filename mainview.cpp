#include "mainview.h"

#include <QKeyEvent>

MainView::MainView(QGraphicsScene *parent) :
    QGraphicsView(parent)
{
}

//void MainView::mousePressEvent(QMouseEvent *event)
//{
//    QGraphicsView::mousePressEvent(event);
//}

void MainView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    } else {
        QGraphicsView::keyPressEvent(event);
    }
}

//void MaintView::
