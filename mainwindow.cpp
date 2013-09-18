#include "mainwindow.h"

#include <QGraphicsLinearLayout>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QLayout>

#include "tidegraphwidget.h"
#include "ButtonWidget.h"
#include "LabelWidget.h"

#include <QPainter>

MainWindow::MainWindow(QGraphicsScene &scene, QGraphicsWidget *parent)
    : QGraphicsWidget(parent)
{
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QGraphicsLinearLayout* windowLayout = new QGraphicsLinearLayout(Qt::Vertical, this);

    setMinimumSize(1424, 600);
    myTideGraph = new TideGraphWidget(this);

    windowLayout->addItem(myTideGraph);

    ButtonWidget* buttonWidget = new ButtonWidget(this);
    buttonWidget->setFlag(QGraphicsItem::ItemIsPanel);
    windowLayout->addItem(buttonWidget);
    QGraphicsLinearLayout* buttonLayout = new QGraphicsLinearLayout(Qt::Horizontal, buttonWidget);
    LabelWidget* labelWidget = new LabelWidget();
    labelWidget->setParent(buttonWidget);
    buttonLayout->addItem(labelWidget);

    connect(myTideGraph, SIGNAL(dateChanged(QDate)), this, SLOT(setDate(QDate)));
}

MainWindow::~MainWindow()
{
    
}

void MainWindow::setDate(const QDate &newDate)
{
    myDateString = newDate.toString();
    update();
}

//void MainWindow::keyPressEvent(QKeyEvent *event)
//{
//    if (event->key() == Qt::Key_Escape)
//    {
//        this->close();
//    } else {
//        myTideGraph->refresh();
//    }
//}

//void MainWindow::mousePressEvent(QGraphicsSceneMouseEvent *event)
//{
//    int i = 0;
//}

void MainWindow::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(Qt::black);
    painter->drawText(0, myTideGraph->size().height() + 50, myDateString);
    //painter->drawImage(0, myTideGraph->size().height(), myTideGraph->image());
}
