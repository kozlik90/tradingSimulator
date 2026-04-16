#ifndef CANDLEDATA_H
#define CANDLEDATA_H
#include "qtypes.h"
struct CandleData {
    qint64 timestamp;
    double openPrice;
    double highPrice;
    double lowPrice;
    double closePrice;
    double volume;
};

#endif // CANDLEDATA_H
