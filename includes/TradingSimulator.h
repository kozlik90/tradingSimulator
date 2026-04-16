#ifndef TRADINGSIMULATOR_H
#define TRADINGSIMULATOR_H
#include <QObject>
#include <QString>
#include "TradeData.h"

class TradingSimulator : public QObject
{
    Q_OBJECT
public:
    TradingSimulator(double bal, QObject*);
    double getBalance();
    void setBalance(double bal) {balance = bal; emit balanceChanged(bal);};
    QList<TradeData> getTrades();
    QList<TradeData> getOpenTrades();
    TradeData getTradeById(int id);
    bool openTrade(QString ticker, TradeType type, double amount, double currentPrice, double takeProfit = 0, double stopLoss = 0);
    bool closeTrade(int tradeId, double currentPrice);
    void updateProfitLoss(QString ticker, double currentPrice);
private:
    double balance;
    QList<TradeData> trades;
    int nextTradeId;
public slots:
    void saveToFile(QString filename);
    void loadFromFile(QString filename);
signals:
    void balanceChanged(double balance);
    void tradeOpened(TradeData trade);
    void tradeClosed(TradeData trade);
    void profitLossUpdated(int tradeId, double profitLoss);

};

class TradingSimulatorException : public std::exception {
private:
    std::string msg;
public:
    TradingSimulatorException(std::string str) : msg(str) {}
    const char* what() const noexcept {return msg.c_str();}
};

#endif // TRADINGSIMULATOR_H
