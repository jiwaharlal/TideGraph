#include "tidegraphwidget.h"

#include <QApplication>
#include <QPainter>
#include <QDir>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QImage>
#include <QPainterPath>
#include <QTime>
#include <QFontMetrics>
#include <QPropertyAnimation>

#include <QElapsedTimer>
#include <QDebug>

static const int NUM_DAYS_IN_CACHE_IMAGE = 3;
static const int SECS_IN_MIN = 60;
static const int MINS_IN_HOUR = 60;
static const int HOURS_IN_DAY = 24;
static const int SECS_IN_HOUR = 3600;
static const int SECS_IN_DAY = SECS_IN_MIN * MINS_IN_HOUR * HOURS_IN_DAY;

static const QColor TIDE_RISE_COLOR(99, 181, 218);
static const QColor TIDE_FALL_COLOR(198, 142, 119);
static const QColor WATER_COLOR(20, 63, 109);

static const QColor GRID_COLOR(176, 217, 234);
static const QColor SIDE_BORDERS_COLOR(176, 217, 234);

static const int LEFT_BORDER_WIDTH = 40;
static const int RIGHT_BORDER_WIDTH = 30;

static const int CURSOR_INFO_RECT_WIDTH = 140;
static const int CURSOR_INFO_RECT_HEIGHT = 55;

static const QFont TIDE_LEVEL_SCALE_FONT("Arial", 12, QFont::Bold);
static const QFont TIME_SCALE_FONT("Arial", 11, QFont::Bold);
static const QFont EXTREM_TAB_TIME_FONT("Arial", 14, QFont::Normal);
static const QFont EXTREM_TAB_LEVEL_FONT("Arial", 14, QFont::Bold);
static const QFont CURSOR_TIME_FONT("Arial", 16, QFont::Normal);
static const QFont CURSOR_TIDE_LEVEL_FONT("Arial", 16, QFont::Bold);

TideGraphWidget::TideGraphWidget(QGraphicsItem *parent)
    : QGraphicsWidget(parent)
    , myTopAreaPixmap(":/img/graph_top_gradient")
    , myHighExtremePixmap(":/img/extreme_high")
    , myLowExtremePixmap(":/img/extreme_low")
    , myTabPixmap(":/img/extrem_tab")
    , myCursorPixmap(":/img/graph_cursor")
    , myLeftMargin(40)
    , myRightMargin(40)
    , myBottomMargin(30)
    , myChartOffset(0)
    , myLastMoveOffset(0)
    , myIsAnimating(false)
{
    assert(!myTopAreaPixmap.isNull());
    setPos(0, 0);
    setMaximumSize(700, 350);
    setMinimumSize(700, 350);

    myTopMargin = myTopAreaPixmap.height();

    QDir dir = QDir::current();
    QString path = dir.absolutePath();
    assert(dir.cd("data"));
    myCache.reset(new TideDataCache(dir.absolutePath().toStdString()));
    myDate = QDate(2013, 10, 4);

    refreshGraph();
}

TideGraphWidget::~TideGraphWidget()
{
}

const QImage &TideGraphWidget::image()
{
    return *myTideGraphImage;
}

void TideGraphWidget::refreshGraph()
{
    myBottomMargin = QFontMetrics(TIME_SCALE_FONT).height() * 3;

    int chartWidth = size().width() - myLeftMargin - myRightMargin;
    int imgChartWidth = chartWidth * 3;
    int imgWidth = imgChartWidth + myLeftMargin + myRightMargin;
    int imgHeight = boundingRect().height();

    myTideGraphImage.reset(new QImage(imgWidth, imgHeight, QImage::Format_ARGB32));
    int chartHeight = imgHeight - myTopMargin - myBottomMargin;

    myVGridCaptionsImage.reset(new QImage(myLeftMargin, chartHeight, QImage::Format_ARGB32));
    myImageChartRect = QRect(
                myLeftMargin,
                myTopMargin,
                imgChartWidth,
                imgHeight - myTopMargin - myBottomMargin);
    myChartScreenRect = QRectF(
                myLeftMargin,
                myTopMargin,
                chartWidth,
                chartHeight);

    myPointList.clear();
    myPointList.reserve(myCache->pointsPerDay());
    for (int i = -1; i <= 1; i++)
    {
        myCache->getPointsForDay(myDate.addDays(i), std::back_inserter<PointList>(myPointList));
    }
    // <debug>
    qDebug() << myPointList[288] << " " << myPointList[289];
    // </debug>

    myPixelsPerPoint = static_cast<qreal>(chartWidth) / myCache->pointsPerDay();

    findExtremes();

    QPainter painter(myTideGraphImage.get());

    QElapsedTimer timer;
    timer.start();
    drawGraphBackground(&painter);
    qDebug() << "bg: " << timer.restart();
    drawWaterGraph(&painter);
    qDebug() << "water: " << timer.restart();
    drawVerticalGrid(&painter);
    qDebug() << "virtical grid: " << timer.restart();
    drawHorizontalGrid(&painter);
    qDebug() << "horizontal grid: " << timer.restart();
    drawExtremes(&painter);
    qDebug() << "extrems: " << timer.restart();
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
    painter->fillRect( QRect( 0, myImageChartRect.bottom(), myTideGraphImage->width(), 1000 ), Qt::white);

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
    painter->setPen(QPen(WATER_COLOR, 1));
    painter->drawPath(path);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->strokePath(whitePath, QPen(Qt::white, 2));
    painter->setRenderHint(QPainter::Antialiasing, false);
}

void TideGraphWidget::drawVerticalGrid(QPainter *painter)
{
    painter->setFont(TIME_SCALE_FONT/*timeFnt*/);

    static const int bottomOverhang = 5;

    QFontMetrics metrics(painter->font());
    qreal fontHeight = metrics.height();
    qreal textWidth = metrics.width("00:00");

    // define grid step
    qreal x1 = timeToImgChartX(QDateTime(myDate));
    qreal x2 = timeToImgChartX(QDateTime(myDate).addSecs(SECS_IN_HOUR));
    int step = (textWidth * 1.5) / (x2 - x1) + 1;

    for (QDateTime dt(myDate.addDays(-1)); dt < QDateTime(myDate.addDays(2)); dt = dt.addSecs(SECS_IN_HOUR * step))
    {
        qreal x = timeToImgChartX(dt);
        qreal textY = myImageChartRect.bottom() + bottomOverhang;
        painter->setPen(GRID_COLOR);
        painter->drawLine(QPointF(x, myImageChartRect.y()), QPointF(x, textY));
        painter->setPen(Qt::black);
        QRectF textRect(x - textWidth / 2, myImageChartRect.bottom(), textWidth, myBottomMargin);
        painter->drawText(textRect, Qt::AlignCenter, dt.time().toString("hh:mm"));
    }
}

void TideGraphWidget::drawHorizontalGrid(QPainter *painter)
{
    QPen solidLinePen( GRID_COLOR, 1 );
    QPen dashedLinePen( solidLinePen );
    QVector< qreal > dashes;
    dashes << 10 << 4;
    dashedLinePen.setDashPattern( dashes );

    QPointF startPoint( myImageChartRect.bottomLeft() );
    QPointF endPoint( myImageChartRect.bottomRight() );
    int HORIZONTAL_GRID_LINES_COUNT = 10;

    double minLocalHeight = myMinTideLevel;
    double minLocalRoundedHeight = static_cast< int >( minLocalHeight /*- 0.5*/ );

    double maxLocalHeight = myMaxTideLevel;
    double maxLocalRoundedHeight = static_cast< int >( maxLocalHeight /*+ 0.5*/ );

    double valIncrement = static_cast< int >( ( maxLocalRoundedHeight - minLocalRoundedHeight ) * 10 / (HORIZONTAL_GRID_LINES_COUNT + 1)  ) / 10.;
    double heightToPixels = myImageChartRect.height() / ( maxLocalHeight - minLocalHeight );

    QPainter gridCaptionsPainter(myVGridCaptionsImage.get());
    gridCaptionsPainter.fillRect(myVGridCaptionsImage->rect(), SIDE_BORDERS_COLOR);
    gridCaptionsPainter.setFont(TIDE_LEVEL_SCALE_FONT);
    QFontMetrics metrics(gridCaptionsPainter.font());
    gridCaptionsPainter.setPen( Qt::black );
    // Draw horizontal grid lines
    for ( double val = minLocalRoundedHeight; val < maxLocalHeight; val += valIncrement )
    {
        QPointF valOffset( 0, heightToPixels * ( val - minLocalHeight ) );
        if ((startPoint - valOffset).y() > myImageChartRect.bottom())
        {
            continue;
        }

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
        QRectF numRect( 0, startPoint.y() - valOffset.y() - metrics.height() / 2, myLeftMargin/*myImageChartRect.left() - TEXT_MARGIN / 2*/, 20 );
        if ( numRect.bottom() < myImageChartRect.bottom() && numRect.top() > myImageChartRect.top() )
        {
            numRect.adjust(0, -myTopMargin * 2, 0, 0);
            gridCaptionsPainter.drawText(numRect, Qt::AlignCenter, QString::number( val, 'f', 1 ) );
        }
    }

}

void TideGraphWidget::drawExtremes(QPainter *painter)
{
    // <debug>

    // <end debug>
    assert(!myTabPixmap.isNull());

    QPen vLinePen(Qt::white);
    vLinePen.setWidth(3);

    // <debug>
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
        painter->setFont( EXTREM_TAB_TIME_FONT );
        painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignBottom, QString( "%1:%2" ).arg( extTime.hour(), 2, 10, QChar('0') ).arg( extTime.minute(), 2, 10, QChar('0') ) );

        textRect.translate( 0, textRect.height() );
        painter->setFont( EXTREM_TAB_LEVEL_FONT );
        QString data = QString::number( it->value , 'f', 2 );
        painter->drawText( textRect, Qt::AlignHCenter | Qt::AlignTop, QString( "%1 %2" ).arg( data, unit ) );
    }
}

void TideGraphWidget::drawCursor(QPainter *painter)
{
    int levelIndex = chartXToPointIndex(myCursorX);
    qreal x = chartXToWidgetX(myCursorX);
    if (! myChartScreenRect.contains(QPointF(x, myChartScreenRect.y() + 1)))
    {
        return;
    }

    QTime cursorTime = chartXToTime(myCursorX);
    double tideLevel = tideLevelAt(QDateTime(myDate).addSecs(QTime(0, 0).secsTo(cursorTime)));
//                myPointList[levelIndex] + myPointList[levelIndex + 1]) / 2;
    qreal y = tideLevelToImageY(tideLevel);
    QPointF imgPos(x - myCursorPixmap.width() / 2, y - myCursorPixmap.height() / 2);
    painter->drawPixmap(imgPos, myCursorPixmap);

    // draw cursor hint
    QPointF cursorPos(x, y);
    QRectF cursorInfoRect( cursorPos - QPointF( CURSOR_INFO_RECT_WIDTH / 2, CURSOR_INFO_RECT_HEIGHT + myCursorPixmap.height() ),
    QSizeF( CURSOR_INFO_RECT_WIDTH, CURSOR_INFO_RECT_HEIGHT ) );
    if ( cursorInfoRect.top() < myChartScreenRect.top() + 30 )
    {
        cursorInfoRect.moveTop( myChartScreenRect.top() + 30 );

        if ( cursorPos.x() < myChartScreenRect.center().x() )
        {
            cursorInfoRect.moveLeft( cursorPos.x() + myCursorPixmap.width() );
        }
        else
        {
            cursorInfoRect.moveRight( cursorPos.x() - myCursorPixmap.width() );
        }
    }

    if ( cursorInfoRect.left() < myChartScreenRect.left() + 10 )
    {
        cursorInfoRect.moveLeft( myChartScreenRect.left() + 10 );
    }
    else if ( cursorInfoRect.right() > myChartScreenRect.right() - 10 )
    {
        cursorInfoRect.moveRight( myChartScreenRect.right() - 10 );
    }

    painter->setPen( QPen( Qt::black, 2 ) );
    painter->setBrush( Qt::white );
    QPainterPath path;
    path.addRect( cursorInfoRect );

    QPointF rectCenter = cursorInfoRect.center();

    if ( ( cursorPos.x() < rectCenter.x() || cursorPos.x() > rectCenter.x() )
        && ( cursorInfoRect.bottom() + myCursorPixmap.height() > cursorPos.y() ) )
    {
        QPointF secondPoint;
        if ( cursorPos.y() > rectCenter.y() )
        {
            secondPoint = rectCenter - QPointF( 0, 20 );
        }
        else
        {
            secondPoint = rectCenter + QPointF( 0, 20 );
        }

        if ( ( cursorPos.x() < rectCenter.x() && cursorPos.y() > rectCenter.y() )
          || ( cursorPos.x() > rectCenter.x() && cursorPos.y() < rectCenter.y() ))
        {
            path.moveTo( rectCenter );
            path.lineTo( cursorPos );
            path.lineTo( secondPoint );
        }
        else
        {
            path.moveTo( secondPoint );
            path.lineTo( cursorPos );
            path.lineTo( rectCenter );
        }
    }
    else
    {
        if ( cursorPos.x() < myChartScreenRect.center().x() )
        {
            path.moveTo( rectCenter );
            path.lineTo( cursorPos );
            path.lineTo( rectCenter - QPointF( 20, 0 ) );
        }
        else
        {
            path.moveTo( rectCenter + QPointF( 20, 0 ) );
            path.lineTo( cursorPos );
            path.lineTo( rectCenter );
        }
    }

    path.setFillRule( Qt::WindingFill );
    painter->drawPath( path.simplified() );

    // Draw date and water height value
    cursorInfoRect.adjust( 0, 5, 0, -cursorInfoRect.height() / 2 );


    painter->setPen( Qt::black );
    painter->setFont( CURSOR_TIME_FONT );
    painter->drawText( cursorInfoRect, Qt::AlignHCenter | Qt::AlignBottom, QString( "%1:%2" ).arg( cursorTime.hour(), 2, 10, QChar('0') ).arg( cursorTime.minute(), 2, 10, QChar('0') ) );

    cursorInfoRect.translate( 0, cursorInfoRect.height() );
    painter->setFont( CURSOR_TIDE_LEVEL_FONT );

    // <debug>
    QString unit = "Mt";
    painter->drawText( cursorInfoRect, Qt::AlignHCenter | Qt::AlignTop, QString("%1 "+unit).arg(tideLevel, 0, 'f', 2) );
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

int TideGraphWidget::chartXToPointIndex(qreal x)
{
    return myPointList.size() * x / myImageChartRect.width();
}

qreal TideGraphWidget::chartXToWidgetX(qreal x)
{
    return x - myChartScreenRect.width() + myChartOffset + myChartScreenRect.x();
}

qreal TideGraphWidget::widgetXToChartX(qreal x)
{
    return x + myChartScreenRect.width() - myChartOffset - myChartScreenRect.x();
}

QTime TideGraphWidget::chartXToTime(qreal x)
{
    qreal secsOnChart = SECS_IN_DAY * NUM_DAYS_IN_CACHE_IMAGE;
    qreal secsFromOrigin = secsOnChart * x / myImageChartRect.width();
    return QDateTime(myDate.addDays(-1)).addSecs(secsFromOrigin).time();
}

double TideGraphWidget::tideLevelAt(const QDateTime &dateTime)
{
    long long secsTo = QDateTime(myDate).addDays(-1).secsTo(dateTime);
    int pointIndex = secsTo * myCache->pointsPerDay() / SECS_IN_DAY;
    qreal secsPerPoint = SECS_IN_DAY / myCache->pointsPerDay();
    qreal t1 = pointIndex * secsPerPoint;
    qreal y1 = myPointList[pointIndex];
    qreal y2 = myPointList[pointIndex + 1];
    return y1 + (y2 - y1) * (secsTo - t1) / secsPerPoint;
}

void TideGraphWidget::setGraphOffset(const qreal &offset)
{
    myChartOffset = offset;
    update();
}

qreal TideGraphWidget::graphOffset() const
{
    return myChartOffset;
}

void TideGraphWidget::privSetDateWithAnimation(const QDate &newDate)
{
    qreal chartDayWidth = boundingRect().width() - myLeftMargin - myRightMargin;
    qreal targetGraphOffset;
    qreal pixelsLeft;
    if (newDate == myDate)
    {
        pixelsLeft = myChartOffset;
        targetGraphOffset = 0;
    }
    else
    {
        pixelsLeft = chartDayWidth - abs(myChartOffset);
        if (newDate < myDate)
        {
            targetGraphOffset = chartDayWidth;
        }
        else
        {
            targetGraphOffset = -chartDayWidth;
        }
    }

    qreal animationDaysPerSec = 2.0;
    qreal duration = abs(pixelsLeft * 1000 / (chartDayWidth * animationDaysPerSec));
    QPropertyAnimation* animation = new QPropertyAnimation(this, "graphOffset");
    animation->setDuration(duration);
    animation->setStartValue(myChartOffset);
    animation->setEndValue(targetGraphOffset);
    connect(animation, SIGNAL(finished()), this, SLOT(onAnimationEnd()));

    myDate = newDate;
    myIsAnimating = true;
    animation->start(QAbstractAnimation::DeleteWhenStopped);
    Q_EMIT dateChanged(newDate);
}


void TideGraphWidget::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QRectF srcRect = boundingRect();
    srcRect.moveTo(myChartScreenRect.width() - myChartOffset, 0);
    painter->drawImage(QPoint(0, 0), *myTideGraphImage, srcRect);
    painter->drawImage(QPoint(0, myTopMargin), *myVGridCaptionsImage);
    painter->fillRect(boundingRect().right() - myRightMargin, myTopMargin, myRightMargin, myImageChartRect.height(), SIDE_BORDERS_COLOR);

    drawCursor(painter);
}

void TideGraphWidget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    if (myIsAnimating)
    {
        return;
    }
    if (QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
        if (!isUnderMouse()) {
            return;
        }
        myPrevMousePos = event->pos();
    }
    else
    {
    }
}

void TideGraphWidget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (myIsAnimating)
    {
        return;
    }
    if (QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
        qreal xOffset = event->pos().x() - myPrevMousePos.x();
        if (abs(xOffset) >= 0.5)
        {
            myLastMoveOffset = xOffset;
        }
        myChartOffset += xOffset;

        myPrevMousePos = event->pos();
        update();
    }
    else
    {
        if (myChartScreenRect.contains(event->pos()))
        {
            myCursorX = widgetXToChartX(event->pos().x());
            update();
        }
    }
}

void TideGraphWidget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if (myIsAnimating)
    {
        return;
    }
    if (QApplication::keyboardModifiers() && Qt::ControlModifier)
    {
        // one-touch behaviour
        if (myLastMoveOffset * myChartOffset > 0)
        {
            if (myChartOffset > 0)
            {
                privSetDateWithAnimation(myDate.addDays(-1));
            }
            else
            {
                privSetDateWithAnimation(myDate.addDays(1));
            }
        }
        else
        {
            privSetDateWithAnimation(myDate);
        }
    }
    else
    {
        // double-touch behaviour
        if (myChartScreenRect.contains(event->pos()))
        {
            myCursorX = widgetXToChartX(event->pos().x());
            update();
        }
    }
}

void TideGraphWidget::onAnimationEnd()
{
    myChartOffset = 0;
    refreshGraph();
    update();
    myIsAnimating = false;
}
