#include "includes/TechnicalIndicators.h"
#include "includes/CandleData.h"

TechnicalIndicators::TechnicalIndicators(QObject* parrent) : QObject(parrent) {

}

QList<double> TechnicalIndicators::calculateMA(QList<CandleData> list, int period)
{
    QList<double> result;
    for(int i = 0; i < list.size(); ++i) {
        double sum = 0;
        int count = 0;
        for(int j = i; j > i - period && j >= 0; --j) {
            sum += list[j].closePrice;
            count++;
        }
        result.push_back(sum / count);
    }
    return result;
}

QList<double> TechnicalIndicators::calculateRSI(QList<CandleData> list, int period)
{
    QList<double> result;

    if (list.size() < period + 1) {
        // Недостаточно данных
        return result;
    }

    // Для каждой свечи начиная с period+1
    for (int i = period; i < list.size(); ++i) {
        double sumGain = 0;
        double sumLoss = 0;

        // Считаем изменения за последние 'period' свечей
        for (int j = i - period + 1; j <= i; ++j) {
            double change = list[j].closePrice - list[j-1].closePrice;

            if (change > 0) {
                sumGain += change;
            } else {
                sumLoss += abs(change);
            }
        }

        double avgGain = sumGain / period;
        double avgLoss = sumLoss / period;

        double RS = (avgLoss == 0) ? 0 : avgGain / avgLoss;
        double RSI = 100 - (100 / (1 + RS));

        result.append(RSI);
    }

    return result;
}

QList<double> TechnicalIndicators::calculateEMA(QList<CandleData> list, int period)
{
    QList<double> result;

    if(list.size() < period) {
        return result;
    }

    double k = 2.0 / (period+1);

    double sum = 0.0;
    for(int i = 0; i < period; ++i) {
        sum += list[i].closePrice;
    }
    double ema = sum / period;
    result.append(ema);

    for(int i = period; i < list.size(); ++i) {
        ema = list[i].closePrice * k + ema * (1 - k);
        result.append(ema);
    }
    return result;
}

QList<double> TechnicalIndicators::calculateEMAFromValues(QList<double> list, int period)
{
    QList<double> result;

    if(list.size() < period) {
        return result;
    }

    double k = 2.0 / (period+1);

    double sum = 0.0;
    for(int i = 0; i < period; ++i) {
        sum += list[i];
    }
    double ema = sum / period;
    result.append(ema);

    for(int i = period; i < list.size(); ++i) {
        ema = list[i] * k + ema * (1 - k);
        result.append(ema);
    }
    return result;
}

QList<MACDData> TechnicalIndicators::calculateMACD(QList<CandleData> list, int fastPeriod, int slowPeriod, int signalPeriod)
{
    QList<MACDData> result;
    QList<double> emaSlow = calculateEMA(list, slowPeriod);
    QList<double> emaFast = calculateEMA(list, fastPeriod);

    if(emaSlow.isEmpty() || emaFast.isEmpty()) {
        return result;
    }

    QList<double> macdLine;
    int slowStart = slowPeriod - 1;  // 25
    int fastStart = fastPeriod - 1;  // 11

    for(int i = 0; i < emaSlow.size(); ++i) {
        int fastIndex = i + (slowStart - fastStart);  // i + 14

        if (fastIndex < emaFast.size()) {
            macdLine.append(emaFast[fastIndex] - emaSlow[i]);
        }
    }

    QList<double> signalLine = calculateEMAFromValues(macdLine, signalPeriod);

    for(int i = 0; i < signalLine.size(); ++i) {
        MACDData data;
        data.macdLine = macdLine[i + signalPeriod-1];
        data.signal = signalLine[i];
        data.histogram = data.macdLine - data.signal;
        result.append(data);
    }

    return result;
}


