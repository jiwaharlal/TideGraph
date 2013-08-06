#include "tidedatacache.h"

#include <fstream>
#include <cassert>

#include <QDate>
#include <QDir>

TideDataCache::TideDataCache(const std::string aDataDirPath)
{
    QDir dir(QString(aDataDirPath.c_str()));
    for (QDate date(2013, 8, 1); date <= QDate(2013, 8, 5); date = date.addDays(1))
    {
        QString filePath = dir.absoluteFilePath(date.toString("yyyy-MM-dd.txt"));
        PointList newList;
        loadDataFromFile(filePath, myDataCache[date]);
    }
}

int TideDataCache::pointsPerDay()
{
    return myDataCache.begin()->second.size();
}

const TideDataCache::PointList &TideDataCache::pointsForDay(const QDate &date) const
{
    CacheMap::const_iterator it = myDataCache.find(reducedDate(date));
    assert(it != myDataCache.end());
    return it->second;
}

QDate TideDataCache::reducedDate(const QDate &date) const
{
    QDate minDate = myDataCache.begin()->first;
    QDate maxDate(minDate);
    for (CacheMap::const_iterator it = myDataCache.begin(); it != myDataCache.end(); ++it)
    {
        QDate pretender = it->first;
        if (pretender < minDate) {
            minDate = pretender;
        } else if (pretender > maxDate) {
            maxDate = pretender;
        }
    }

    return minDate.addDays(minDate.daysTo(date) % minDate.daysTo(maxDate));
}

void TideDataCache::loadDataFromFile(const QString &fileName, TideDataCache::PointList &outPointList)
{
    outPointList.clear();
    std::ifstream in(fileName.toAscii(), std::ifstream::in);
    assert(in.is_open());
    double tempValue;
    while (in.good())
    {
        in >> tempValue;
        if (in.good())
            outPointList.push_back(tempValue);
    }
    in.close();
}
