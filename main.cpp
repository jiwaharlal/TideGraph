#include "mainwindow.h"
#include <QApplication>
#include <QtGui>

#include "mainview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QGraphicsScene scene;
    MainWindow* w = new MainWindow(scene);
    scene.addItem(w);
    MainView view(&scene);
    view.resize(1429, 605);
    view.show();
    
    return a.exec();
}
