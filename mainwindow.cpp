#include "mainwindow.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QLayout>

#include "tidegraphwidget.h"

MainWindow::MainWindow(QGraphicsWidget *parent)
    : QGraphicsWidget(parent)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QGraphicsLinearLayout* windowLayout = new QGraphicsLinearLayout(this);

    setMinimumSize(1024, 600);
    myTideGraph = new TideGraphWidget();

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
    }
}
