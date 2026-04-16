#ifndef TRADINGSIGNALS_H
#define TRADINGSIGNALS_H

#include <QObject>
#include <QString>

struct SignalAnalysis {
    double currentPrice;
    double maValue;
    double rsiValue;
    double macdValue;
    double signalValue;

    int maScore;
    int rsiScore;
    int macdScore;

    int totalScore;
    QString recommendation;
    QString reasoning;


};

class TradingSignals : public QObject
{
    Q_OBJECT
public:
    TradingSignals(QObject*);
    static SignalAnalysis analyzeSignals(double currentPrice, double maValue, double rsiValue, double macdLine, double signalLine);
};

#endif // TRADINGSIGNALS_H
