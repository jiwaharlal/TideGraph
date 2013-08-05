#include "mainwindow.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QLayout>

#include "tidegraphwidget.h"

#include <QPainter>

MainWindow::MainWindow(QGraphicsScene &scene, QGraphicsWidget *parent)
    : QGraphicsWidget(parent)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QGraphicsLinearLayout* windowLayout = new QGraphicsLinearLayout(this);

    setMinimumSize(1424, 600);
    myTideGraph = new TideGraphWidget(this);
    //scene.addItem(this);

    windowLayout->addItem(myTideGraph);
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    } else {
        myTideGraph->refresh();
    }
}

void MainWindow::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    int i = 0;
}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(0, myTideGraph->size().height(), myTideGraph->image());
}
