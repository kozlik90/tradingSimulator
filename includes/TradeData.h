#include <QString>
#include <QDateTime>
#ifndef TRADEDATA_H
#define TRADEDATA_H

enum class TradeType {
    BUY,
    SELL
};

enum class TradeStatus {
    OPEN,
    CLOSED
};

struct TradeData {
    int id;
    QString ticker;
    TradeType type;
    double entryPrice;
    double amount;
    double quantity;
    double takeProfit;
    double stopLoss;
    QDateTime openTime;
    QDateTime closeTime;
    double closePrice;
    TradeStatus status;
    double profitLoss;
};

#endif // TRADEDATA_H
