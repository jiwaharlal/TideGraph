#ifndef TIDEGRAPHWIDGET_H
#define TIDEGRAPHWIDGET_H

#include <QGraphicsWidget>

#include "rl_ptr.h"

#include "tidedatacache.h"

class QImage;

struct Extreme
{
    enum Type {
        Minimum,
        Maximum
    };

    Type        type;
    QDateTime   dateTime;
    double      value;
};

class TideGraphWidget : public QGraphicsWidget
{
    Q_OBJECT
public:
    explicit TideGraphWidget(QGraphicsItem *parent = 0);
    virtual ~TideGraphWidget();
    
protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /* = 0 */) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

protected:
signals:
    
public slots:

private:
    void refreshGraph();
    void findExtremes();
    void drawGraphBackground(QPainter *painter);
    void drawWaterGraph(QPainter* painter);
    void drawVerticalGrid(QPainter* painter);
    void drawExtremes(QPainter* painter);

    int timeToImageX(const QDateTime& dateTime);
    qreal tideLevelToImageY(double tideLevel);
private:
    QPixmap myUpExtremePixmap;
    QPixmap myTabPixmap;

    TideDataCache* myCache;
    QDate myDate;

    typedef std::vector<double> PointList;
    PointList myPointList;

    typedef std::vector<Extreme> ExtremeList;
    ExtremeList myExtremeList;

    double myMinTideLevel;
    double myMaxTideLevel;

    koki::rl_ptr<QImage> myTideGraphImage;
    qreal myPixelsPerPointInterval;
};

#endif // TIDEGRAPHWIDGET_H
