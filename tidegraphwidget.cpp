#include "tidegraphwidget.h"

#include <QPainter>
#include <QDir>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QImage>
#include <QPainterPath>
#include <QTime>

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

static const QColor GRID_COLOR(176, 217, 234);

static const int LEFT_BORDER_WIDTH = 40;
static const int RIGHT_BORDER_WIDTH = 30;

TideGraphWidget::TideGraphWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , myTopAreaPixmap(":/img/graph_top_gradient")
    , myHighExtremePixmap(":/img/extreme_high")
    , myLowExtremePixmap(":/img/extreme_low")
    , myTabPixmap(":/img/extrem_tab")
    , myCache(NULL)
    , myLeftBorderWidth(40)
    , myRightBorderWidth(30)
    , myBottomBorderHeight(30)
{
    assert(!myTopAreaPixmap.isNull());
    setPos(0, 0);
    setMaximumSize(500, 300);
    setMinimumSize(500, 300);

    myTopBorderHeight = myTopAreaPixmap.height();

    QDir dir = QDir::current();
    QString path = dir.absolutePath();
    assert(dir.cd("data"));
    myCache = new TideDataCache(dir.absolutePath().toStdString());
    myDate = QDate(2013, 10, 4);

    refreshGraph();
}

TideGraphWidget::~TideGraphWidget()
{
    delete myCache;
    myCache = NULL;
}

const QImage &TideGraphWidget::image()
{
    return *myTideGraphImage;
}

void TideGraphWidget::refresh()
{
    refreshGraph();\
    update();
}

void TideGraphWidget::refreshGraph()
{
    int chartWidth = size().width() - myLeftBorderWidth - myRightBorderWidth;
    int imgChartWidth = chartWidth * 3;
    int imgWidth = imgChartWidth + myLeftBorderWidth + myRightBorderWidth;
    int imgHeight = boundingRect().height();
    myTideGraphImage.reset(new QImage(imgWidth, imgHeight, QImage::Format_ARGB32));
    int chartHeight = imgHeight - myTopBorderHeight - myBottomBorderHeight;
    myVGridCaptionsImage.reset(new QImage(myLeftBorderWidth, chartHeight, QImage::Format_ARGB32));
    myImageChartRect = QRect(
                myLeftBorderWidth,
                myTopBorderHeight,
                imgChartWidth,
                imgHeight - myTopBorderHeight - myBottomBorderHeight);

    myPointList.clear();
    myPointList.reserve(myCache->pointsPerDay());
    for (int i = -1; i <= 1; i++)
    {
        myCache->getPointsForDay(myDate.addDays(i), std::back_inserter<PointList>(myPointList));
    }

    myPixelsPerPoint = static_cast<qreal>(chartWidth) / myCache->pointsPerDay();

    findExtremes();

    QPainter painter(myTideGraphImage.get());
    painter.setRenderHint(QPainter::Antialiasing);

    drawGraphBackground(&painter);
    drawWaterGraph(&painter);
    drawVerticalGrid(&painter);
    drawHorizontalGrid(&painter);
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
    painter->fillRect( QRect( 0, 0, myTideGraphImage->width(), myTopAreaPixmap.height() ), QBrush( myTopAreaPixmap ) );
    painter->fillRect( QRect( 0, myImageChartRect.bottom(), myTideGraphImage->width(), 1000 ), Qt::lightGray);

    int rectLeft = myImageChartRect.left();
    int rectRight;
    for (ExtremeList::const_iterator it = myExtremeList.begin(); it != myExtremeList.end(); ++it)
    {
        rectRight = timeToImgChartX(it->dateTime);
        QColor color = it->type == Extreme::Maximum ? TIDE_RISE_COLOR : TIDE_FALL_COLOR;
        painter->fillRect(rectLeft, myImageChartRect.y(), rectRight - rectLeft, myImageChartRect.height(), color);
        rectLeft = rectRight;
    }
    rectRight = timeToImgChartX(QDateTime(myDate.addDays(2)));
    QColor color = myExtremeList.back().type == Extreme::Minimum ? TIDE_RISE_COLOR : TIDE_FALL_COLOR;
    painter->fillRect(rectLeft,  myImageChartRect.y(), rectRight - rectLeft, myImageChartRect.height(), color);
}

void TideGraphWidget::drawWaterGraph(QPainter* painter)
{
    QPainterPath path;
    PointList::const_iterator it = myPointList.begin();
    path.moveTo(myImageChartRect.x(), tideLevelToImageY(*it));
    qreal imgX = myImageChartRect.x();
    for (++it; it != myPointList.end(); ++it)
    {
        imgX += myPixelsPerPoint;
        path.lineTo(imgX, tideLevelToImageY(*it));
    }
    QPainterPath whitePath(path);
    path.lineTo(myImageChartRect.bottomRight());
    path.lineTo(myImageChartRect.bottomLeft());
    path.closeSubpath();

    painter->setBrush(WATER_COLOR);
    painter->setPen(QPen(WATER_COLOR, 2));
    painter->drawPath(path);
    painter->strokePath(whitePath, QPen(Qt::white, 2));
}

void TideGraphWidget::drawVerticalGrid(QPainter *painter)
{
    static const int bottomOverhang = 5;

    QFontMetrics metrics(painter->font());
    qreal fontHeight = metrics.height();
    qreal textWidth = metrics.width("00:00");

    // define grid step
    qreal x1 = timeToImgChartX(QDateTime(myDate));
    qreal x2 = timeToImgChartX(QDateTime(myDate).addSecs(SECS_IN_HOUR));
    int step = (textWidth * 1.5) / (x2 - x1) + 1;

    painter->setPen(Qt::white);
    for (QDateTime dt(myDate.addDays(-1)); dt < QDateTime(myDate.addDays(2)); dt = dt.addSecs(SECS_IN_HOUR * step))
    {
        qreal x = timeToImgChartX(dt);
        qreal textY = myImageChartRect.bottom() + bottomOverhang;
        painter->drawLine(QPointF(x, myImageChartRect.y()), QPointF(x, textY));
        painter->drawText(x - textWidth / 2, textY + 10, dt.time().toString("hh:mm"));
    }

}

void TideGraphWidget::drawHorizontalGrid(QPainter *painter)
{
    QPen solidLinePen( QColor( 176, 217, 234 ), 1 );
    QPen dashedLinePen( solidLinePen );
    QVector< qreal > dashes;
    dashes << 10 << 4;
    dashedLinePen.setDashPattern( dashes );

    QPointF startPoint( myImageChartRect.bottomLeft() );
    QPointF endPoint( myImageChartRect.bottomRight() );
    int HORIZONTAL_GRID_LINES_COUNT = 10;

    double minLocalHeight = myMinTideLevel;
    double minLocalRoundedHeight = static_cast< int >( minLocalHeight - 0.5 );

    double maxLocalHeight = myMaxTideLevel;
    double maxLocalRoundedHeight = static_cast< int >( maxLocalHeight + 0.5 );

    double valIncrement = static_cast< int >( ( maxLocalRoundedHeight - minLocalRoundedHeight ) * 10 / (HORIZONTAL_GRID_LINES_COUNT + 1)  ) / 10.;
    double heightToPixels = myImageChartRect.height() / ( maxLocalHeight - minLocalHeight );

    QPainter gridCaptionsPainter(myVGridCaptionsImage.get());
    gridCaptionsPainter.fillRect(myVGridCaptionsImage->rect(), QColor(200, 200, 240));
    // Draw horizontal grid lines
    for ( double val = minLocalRoundedHeight; val < maxLocalHeight; val += valIncrement )
    {
        QPointF valOffset( 0, heightToPixels * ( val - minLocalHeight ) );

        if ( qAbs( val ) < 0.01 )
        {
            painter->setPen( solidLinePen );
        }
        else
        {
            painter->setPen( dashedLinePen );
        }
        painter->drawLine( startPoint - valOffset, endPoint - valOffset );

        int TEXT_MARGIN = 5;
        QRectF numRect( 0, startPoint.y() - valOffset.y() - 10, myImageChartRect.left() - TEXT_MARGIN / 2, 20 );
        if ( numRect.bottom() < myImageChartRect.bottom() && numRect.top() > myImageChartRect.top() )
        {
            painter->setPen( Qt::black );
            painter->drawText( numRect, Qt::AlignVCenter | Qt::AlignRight, QString::number( val, 'f', 1 ) );
            numRect.adjust(0, -myTopBorderHeight, 0, 0);
            gridCaptionsPainter.drawText(numRect, Qt::AlignVCenter | Qt::AlignRight, QString::number( val, 'f', 1 ) );
        }
    }

}

void TideGraphWidget::drawExtremes(QPainter *painter)
{
    assert(!myTabPixmap.isNull());

    QFont timeFnt("Arial");
    timeFnt.setBold( false );
    timeFnt.setPixelSize( 20 );

    QFont heightFnt("Arial");
    heightFnt.setBold( true );
    heightFnt.setPixelSize( 20 );

    QPen vLinePen(Qt::yellow);
    vLinePen.setWidth(2);
    painter->setPen(vLinePen);

    QString unit("m");
    for (ExtremeList::const_iterator it = myExtremeList.begin(); it != myExtremeList.end(); ++it)
    {
        painter->setPen(vLinePen);
        qreal x = timeToImgChartX(it->dateTime);
        painter->drawLine(x, myImageChartRect.y(), x, myImageChartRect.bottom());
        if (it->type == Extreme::Maximum)
        {
            qreal imageX = x - (myHighExtremePixmap.width() / 2);
            painter->drawPixmap(imageX, myImageChartRect.y(), myHighExtremePixmap);
        }
        else
        {
            qreal imageX = x - (myLowExtremePixmap.width() / 2);
            painter->drawPixmap(imageX, myImageChartRect.y(), myLowExtremePixmap);
        }

        // Draw tab
        QPointF extremTabPos( x - myTabPixmap.width() / 2, myImageChartRect.top() - myTabPixmap.height() );
        painter->drawPixmap(extremTabPos, myTabPixmap);
        // Draw extrema time and value
        QRectF textRect( extremTabPos, myTabPixmap.size() );
        textRect.adjust( 0, 5, 0, -myTabPixmap.height() / 2 );

        QTime extTime( it->dateTime.time() );
        painter->setPen( Qt::black );
        painter->setFont( timeFnt );
        painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignBottom, QString( "%1:%2" ).arg( extTime.hour(), 2, 10, QChar('0') ).arg( extTime.minute(), 2, 10, QChar('0') ) );

        textRect.translate( 0, textRect.height() );
        painter->setFont( heightFnt );
        QString data = QString::number( it->value , 'f', 2 );
        painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, QString( "%1 %2" ).arg( data, unit ) );
    }
}

int TideGraphWidget::timeToImgChartX(const QDateTime &dateTime)
{
    long long secsOnGraph = NUM_DAYS_IN_CACHE_IMAGE * SECS_IN_DAY;
    long long secsTo = QDateTime(myDate).addDays(-1).secsTo(dateTime);
    return myImageChartRect.x() + myImageChartRect.width() * secsTo / secsOnGraph;
}

qreal TideGraphWidget::tideLevelToImageY(double tideLevel)
{
    qreal bottomMargin = myImageChartRect.height() * .05;
    qreal topMargin = myHighExtremePixmap.height() + bottomMargin;
    qreal verticalRange = myImageChartRect.height() - topMargin - bottomMargin;

    qreal tideDiff = myMaxTideLevel - myMinTideLevel;
    qreal yInChart = (tideLevel - myMinTideLevel) * verticalRange / tideDiff;
    qreal y = myImageChartRect.bottom() - bottomMargin - yInChart;

    return y;
}

void TideGraphWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->drawImage(QPoint(0, 0), *myTideGraphImage, boundingRect());
    painter->drawImage(QPoint(0, myTopBorderHeight), *myVGridCaptionsImage);
}

void TideGraphWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    /*if (!isUnderMouse()) {
        return;
    }
    if (event->pos().x() < size().width() / 2) {
        myDate.addDays(-1);
    } else {
        myDate.addDays(1);
    }*/
    refreshGraph();
}
