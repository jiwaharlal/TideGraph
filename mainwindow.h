#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGraphicsWidget>

class TideGraphWidget;

class MainWindow : public QGraphicsWidget
{
    Q_OBJECT
    
public:
    MainWindow(QGraphicsWidget *parent = 0);
    ~MainWindow();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    TideGraphWidget* myTideGraph;
};

#endif // MAINWINDOW_H
