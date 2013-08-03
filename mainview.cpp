#include "mainview.h"

#include <QKeyEvent>

MainView::MainView(QGraphicsScene *parent) :
    QGraphicsView(parent)
{
}

void MainView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
}
