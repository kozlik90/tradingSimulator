#include "includes/TradingSignals.h"

TradingSignals::TradingSignals(QObject* parrent) : QObject(parrent) {

}

SignalAnalysis TradingSignals::analyzeSignals(double currentPrice, double maValue, double rsiValue, double macdLine, double signalLine)
{
    SignalAnalysis result;
    result.currentPrice = currentPrice;
    result.maValue = maValue;
    result.rsiValue = rsiValue;
    result.macdValue = macdLine;
    result.signalValue = signalLine;

    double priceToMaRatio = (currentPrice - maValue) / maValue * 100;
    if (priceToMaRatio > 2) result.maScore = 2;       // Цена намного выше MA
    else if (priceToMaRatio > 0.5) result.maScore = 1;  // Цена выше MA
    else if (priceToMaRatio > -0.5) result.maScore = 0; // Около MA
    else if (priceToMaRatio > -2) result.maScore = -1;  // Цена ниже MA
    else result.maScore = -2;                           // Цена намного ниже MA

    if (rsiValue < 30) result.rsiScore = 2;        // Перепродан - BUY
    else if (rsiValue < 40) result.rsiScore = 1;   // Близко к перепроданности
    else if (rsiValue < 60) result.rsiScore = 0;   // Нейтрально
    else if (rsiValue < 70) result.rsiScore = -1;  // Близко к перекупленности
    else result.rsiScore = -2;                     // Перекуплен - SELL

    double macdDiff = macdLine - signalLine;  // Разница между линиями

    if (macdDiff > 10) {
        // MACD намного выше Signal - сильный восходящий тренд
        result.macdScore = 2;
    } else if (macdDiff > 0) {
        // MACD выше Signal - восходящий тренд
        result.macdScore = 1;
    } else if (macdDiff > -10) {
        // MACD ниже Signal - нисходящий тренд
        result.macdScore = -1;
    } else {
        // MACD намного ниже Signal - сильный нисходящий тренд
        result.macdScore = -2;
    }

    result.totalScore = result.maScore + result.rsiScore + result.macdScore;

    // 5. Рекомендация
    if (result.totalScore > 3) {
        result.recommendation = "STRONG BUY";
        result.reasoning = "Все индикаторы указывают на восходящий тренд";
    } else if (result.totalScore > 1) {
        result.recommendation = "BUY";
        result.reasoning = "Индикаторы склоняются к покупке";
    } else if (result.totalScore < -3) {
        result.recommendation = "STRONG SELL";
        result.reasoning = "Все индикаторы указывают на нисходящий тренд";
    } else if (result.totalScore < -1) {
        result.recommendation = "SELL";
        result.reasoning = "Индикаторы склоняются к продаже";
    } else {
        result.recommendation = "HOLD";
        result.reasoning = "Неопределённая ситуация, лучше подождать";
    }

    return result;
}
