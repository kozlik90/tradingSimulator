#ifndef TECHNICALINDICATORS_H
#define TECHNICALINDICATORS_H

#include <QObject>

struct CandleData;
struct MACDData {
    double macdLine;
    double signal;
    double histogram;
};

class TechnicalIndicators : public QObject
{
    Q_OBJECT
public:
    TechnicalIndicators(QObject*);
    static QList<double> calculateMA(QList<CandleData>, int);
    static QList<double> calculateRSI(QList<CandleData>, int period = 14);
    static QList<double> calculateEMA(QList<CandleData>, int period);
    static QList<double> calculateEMAFromValues(QList<double>, int period);
    static QList<MACDData> calculateMACD(QList<CandleData> list, int fastPeriod = 12, int slowPeriod = 26, int signalPeriod = 9);
};

#endif // TECHNICALINDICATORS_H
