#include "tidegraphwidget.h"

#include <QPainter>
#include <QDir>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QImage>
#include <QPainterPath>

static const int NUM_DAYS_IN_CACHE_IMAGE = 3;
static const int MSECS_IN_SEC = 1000;
static const int SECS_IN_MIN = 60;
static const int MINS_IN_HOUR = 60;
static const int HOURS_IN_DAY = 24;
static const int SECS_IN_HOUR = 3600;
static const int SECS_IN_DAY = SECS_IN_MIN * MINS_IN_HOUR * HOURS_IN_DAY;

static const QColor TIDE_RISE_COLOR(200, 100, 100);
static const QColor TIDE_FALL_COLOR(100, 100, 200);
static const QColor WATER_COLOR(50, 50, 200);

TideGraphWidget::TideGraphWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , myUpExtremePixmap(":/img/up_extreme")
    , myTabPixmap(":/img/tab")
    , myCache(NULL)
{
    setPos(0, 0);
    setMaximumSize(800, 300);
    setMinimumSize(800, 300);

    QDir dir = QDir::current();
    assert(dir.cd("../data"));
    myCache = new TideDataCache(dir.absolutePath().toStdString());
    myDate = QDate(2013, 8, 2);

    refreshGraph();
}

TideGraphWidget::~TideGraphWidget()
{
    delete myCache;
    myCache = NULL;
}

void TideGraphWidget::refreshGraph()
{
    myPointList.clear();
    myPointList.reserve(myCache->pointsPerDay());
    for (int i = -1; i <= 1; i++)
    {
        myCache->getPointsForDay(myDate.addDays(i), std::back_inserter<PointList>(myPointList));
    }

    myTideGraphImage.reset(new QImage(700 * 3, 250, QImage::Format_ARGB32));
    myPixelsPerPointInterval = static_cast<qreal>(700) / myCache->pointsPerDay();
    QPainter painter(myTideGraphImage.get());
    painter.setRenderHint(QPainter::Antialiasing);
    findExtremes();
    drawGraphBackground(&painter);
    drawWaterGraph(&painter);
    drawVerticalGrid(&painter);
    drawExtremes(&painter);
}

void TideGraphWidget::findExtremes()
{
    myExtremeList.clear();
    myMaxTideLevel = myMinTideLevel = myPointList.front();
    PointList::iterator pointIt = myPointList.begin() + 1;
    double prevDelta = *pointIt - *(myPointList.begin());
    for ( ; pointIt + 1 != myPointList.end(); ++pointIt)
    {
        double delta = *(pointIt + 1) - *pointIt;
        if (delta * prevDelta < 0)
        {
            Extreme x;
            x.type = delta < 0 ? Extreme::Maximum : Extreme::Minimum;
            x.value = *pointIt;
            int pointIndex = pointIt - myPointList.begin();
            x.dateTime = QDateTime(myDate).addDays(-1).addSecs(pointIndex * SECS_IN_DAY / myCache->pointsPerDay());

            if (x.value > myMaxTideLevel)
            {
                myMaxTideLevel = x.value;
            }
            else if (x.value < myMinTideLevel)
            {
                myMinTideLevel = x.value;
            }

            myExtremeList.push_back(x);
        }
        prevDelta = delta;
    }
}

void TideGraphWidget::drawGraphBackground(QPainter* painter)
{
    int rectLeft = 0;
    int rectRight;
    for (ExtremeList::const_iterator it = myExtremeList.begin(); it != myExtremeList.end(); ++it)
    {
        rectRight = timeToImageX(it->dateTime);
        QColor color = it->type == Extreme::Maximum ? TIDE_RISE_COLOR : TIDE_FALL_COLOR;
        painter->fillRect(rectLeft, 0, rectRight - rectLeft, myTideGraphImage->height(), color);
        rectLeft = rectRight;
    }
    rectRight = timeToImageX(QDateTime(myDate.addDays(2)));
    QColor color = myExtremeList.back().type == Extreme::Minimum ? TIDE_RISE_COLOR : TIDE_FALL_COLOR;
    painter->fillRect(rectLeft, 0, rectRight - rectLeft, myTideGraphImage->height(), color);
}

void TideGraphWidget::drawWaterGraph(QPainter* painter)
{
    QPainterPath path;
    path.moveTo(0, myTideGraphImage->height());
    PointList::const_iterator it = myPointList.begin();
    path.lineTo(0, tideLevelToImageY(*it));
    qreal imgX = 0;
    for (++it; it != myPointList.end(); ++it)
    {
        imgX += myPixelsPerPointInterval;
        path.lineTo(imgX, tideLevelToImageY(*it));
    }
    path.lineTo(myTideGraphImage->width(), myTideGraphImage->height());
    path.closeSubpath();
    painter->fillPath(path, WATER_COLOR);
}

void TideGraphWidget::drawVerticalGrid(QPainter *painter)
{
    for (QDateTime dt(myDate.addDays(-1)); dt < QDateTime(myDate.addDays(2)); dt = dt.addSecs(SECS_IN_HOUR))
    {
        qreal x = timeToImageX(dt);
        QPen pen(Qt::white);
        painter->setPen(Qt::white);
        QFontMetrics metrics(painter->font());
        qreal fontHeight = metrics.height();
        painter->drawLine(QPointF(x, 0), QPointF(x, myTideGraphImage->height() - fontHeight));

    }

}

int TideGraphWidget::timeToImageX(const QDateTime &dateTime)
{
    long long secsOnGraph = NUM_DAYS_IN_CACHE_IMAGE * SECS_IN_DAY;
    long long secsTo = QDateTime(myDate).addDays(-1).secsTo(dateTime);
    return myTideGraphImage->size().width() * secsTo / secsOnGraph;
}

qreal TideGraphWidget::tideLevelToImageY(double tideLevel)
{
    qreal topMargin = 10;
    qreal bottomMargin = 10;
    qreal imgHeight = myTideGraphImage->height();
    qreal tideDiff = myMaxTideLevel - myMinTideLevel;

    qreal result = imgHeight - ((tideLevel - myMinTideLevel) * (imgHeight - topMargin - bottomMargin) / tideDiff + bottomMargin);
    return result;
}

void TideGraphWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawImage(0, 0, *myTideGraphImage);
//    painter->fillRect(QRectF(QPointF(0, 0), size()), QColor(150, 150, 159));
//    painter->drawPixmap(0, 0, myUpExtremePixmap);
//    QPoint tabPos(50, 50);
//    painter->drawPixmap(tabPos, myTabPixmap);
//    QRectF textRect(QPointF(tabPos), myTabPixmap.size());
//    QFont font = painter->font();
//    font.setPixelSize(28);
//    painter->setFont(font);
//    painter->drawText(textRect, Qt::AlignCenter, "42");
}

void TideGraphWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (!isUnderMouse()) {
        return;
    }
    if (event->pos().x() < size().width() / 2) {
        myDate.addDays(-1);
    } else {
        myDate.addDays(1);
    }
    refreshGraph();
}
