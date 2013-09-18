#ifndef TIDEDATACACHE_H
#define TIDEDATACACHE_H

#include <vector>
#include <map>
#include <algorithm>
#include <cassert>
#include <string>

#include <QDate>

class TideDataCache
{
public:
    typedef std::vector<double> PointList;

    TideDataCache(const std::string aDataDirPath);

    int pointsPerDay();

    template <class Iterator>
    void getPointsForDay(const QDate& date, Iterator outIt);
    const PointList& pointsForDay(const QDate& date) const;
private:
    typedef std::map<QDate, PointList> CacheMap;

    QDate reducedDate(const QDate& date) const;
    void loadDataFromFile(const QString& fileName, PointList& outPointList);

private:
    CacheMap myDataCache;
};

template <class Iterator>
void TideDataCache::getPointsForDay(const QDate& date, Iterator outIt)
{
    QDate indexDate = reducedDate(date);
    TideDataCache::CacheMap::const_iterator it = myDataCache.find(indexDate);
    assert(it != myDataCache.end());
    std::copy(it->second.begin(), it->second.end(), outIt);
}

#endif // TIDEDATACACHE_H
