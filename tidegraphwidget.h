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

    const QImage& image();
    void refresh();
    
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
    void drawHorizontalGrid(QPainter* painter);
    void drawExtremes(QPainter* painter);

    int timeToImgChartX(const QDateTime& dateTime);
    qreal tideLevelToImageY(double tideLevel);
private:
    koki::rl_ptr<QImage> myTideGraphImage;
    koki::rl_ptr<QImage> myVGridCaptionsImage;

    QRect myImageChartRect;

    int myLeftBorderWidth;
    int myRightBorderWidth;
    int myTopBorderHeight;
    int myBottomBorderHeight;

    QPixmap myTopAreaPixmap;
    QPixmap myHighExtremePixmap;
    QPixmap myLowExtremePixmap;
    QPixmap myTabPixmap;
    QPixmap myCursorPixmap;

    TideDataCache* myCache;
    QDate myDate;

    typedef std::vector<double> PointList;
    PointList myPointList;

    typedef std::vector<Extreme> ExtremeList;
    ExtremeList myExtremeList;

    double myMinTideLevel;
    double myMaxTideLevel;

    qreal myPixelsPerPoint;
};

#endif // TIDEGRAPHWIDGET_H
