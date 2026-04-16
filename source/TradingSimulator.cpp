#include "includes/TradingSimulator.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QByteArray>


TradingSimulator::TradingSimulator(double bal, QObject* parrent) : QObject(parrent), balance(bal)
{
    nextTradeId = 0;
}

double TradingSimulator::getBalance()
{
    return balance;
}

QList<TradeData> TradingSimulator::getTrades()
{
    return trades;
}

QList<TradeData> TradingSimulator::getOpenTrades()
{
    QList<TradeData> result;
    foreach(TradeData item, trades) {
        if(item.status == TradeStatus::OPEN) {
            result.append(item);
        }
    }
    return result;
}

TradeData TradingSimulator::getTradeById(int id)
{
    TradeData trade;
    for(int i = 0; i < trades.size(); ++i) {
        if(trades[i].id == id) {
            return trades[i];
        }
    }
    return trade;
}

bool TradingSimulator::openTrade(QString ticker, TradeType type, double amount, double currentPrice, double takeProfit, double stopLoss)
{
    if(balance < amount) {
        return false;
    }
    if(type == TradeType::BUY) {
        if(takeProfit < currentPrice && takeProfit != 0)
            throw TradingSimulatorException("Take Profit lower then current price");
        if(stopLoss > currentPrice && stopLoss != 0)
            throw TradingSimulatorException("Stop Loss higher then current price");
    }
    else if(type == TradeType::SELL) {
        if(takeProfit > currentPrice && takeProfit != 0)
            throw TradingSimulatorException("Take Profit higher then current price");
        if(stopLoss < currentPrice && stopLoss != 0)
            throw TradingSimulatorException("Stop Loss lower then current price");
    }
    if(amount < 0) {
        throw TradingSimulatorException("Amount  must be positive");
    }
    TradeData trade;
    trade.id = nextTradeId;
    trade.ticker = ticker;
    trade.type = type;
    trade.amount = amount;
    balance -= amount;
    emit balanceChanged(balance);
    trade.entryPrice = currentPrice;
    trade.takeProfit = takeProfit;
    trade.stopLoss = stopLoss;
    trade.profitLoss = 0;
    trade.quantity = amount / currentPrice;
    trade.openTime = QDateTime::currentDateTime();
    trade.status = TradeStatus::OPEN;
    trades.append(trade);
    emit tradeOpened(trade);
    nextTradeId++;
    return true;
}

bool TradingSimulator::closeTrade(int tradeId, double currentPrice)
{
    for(int i = 0; i < trades.size(); ++i) {
        if(trades[i].id == tradeId) {
            trades[i].closePrice = currentPrice;
            if(trades[i].type == TradeType::BUY) {
                trades[i].profitLoss = (currentPrice - trades[i].entryPrice) * trades[i].quantity;
            }
            else trades[i].profitLoss = -(currentPrice - trades[i].entryPrice) * trades[i].quantity;
            trades[i].status = TradeStatus::CLOSED;
            trades[i].closeTime = QDateTime::currentDateTime();
            balance += trades[i].amount + trades[i].profitLoss;
            emit balanceChanged(balance);
            emit tradeClosed(trades[i]);
            emit profitLossUpdated(trades[i].id, trades[i].profitLoss);
            return true;
        }
    }
    return false;
}

void TradingSimulator::updateProfitLoss(QString ticker, double currentPrice)
{
    for(int i = 0; i < trades.size(); ++i) {
        if(trades[i].ticker == ticker && trades[i].status == TradeStatus::OPEN) {
            if(trades[i].type == TradeType::BUY) {
                trades[i].profitLoss = (currentPrice - trades[i].entryPrice) * trades[i].quantity;
                if(currentPrice >= trades[i].takeProfit && trades[i].takeProfit != 0) {
                    closeTrade(trades[i].id, currentPrice);
                }
                if(currentPrice <= trades[i].stopLoss && trades[i].stopLoss != 0) {
                    closeTrade(trades[i].id, currentPrice);
                }
            }
            else {
                trades[i].profitLoss = -(currentPrice - trades[i].entryPrice) * trades[i].quantity;
                if(currentPrice <= trades[i].takeProfit && trades[i].takeProfit != 0) {
                    closeTrade(trades[i].id, currentPrice);
                }
                if(currentPrice >= trades[i].stopLoss && trades[i].stopLoss != 0) {
                    closeTrade(trades[i].id, currentPrice);
                }
            }

            emit profitLossUpdated(trades[i].id, trades[i].profitLoss);
        }
    }
}

void TradingSimulator::saveToFile(QString filename)
{
    QJsonObject obj;
    obj["balance"] = balance;
    obj["nextTradeId"] = nextTradeId;

    QJsonArray array;
    for(int i = 0; i < trades.size(); ++i) {
        QJsonObject trade;
        trade["id"] = trades[i].id;
        trade["ticker"] = trades[i].ticker;
        trade["type"] = (trades[i].type == TradeType::BUY) ? "BUY" : "SELL";
        trade["entryPrice"] = trades[i].entryPrice;
        trade["amount"] = trades[i].amount;
        trade["quantity"] = trades[i].quantity;
        trade["takeProfit"] = trades[i].takeProfit;
        trade["stopLoss"] = trades[i].stopLoss;
        trade["openTime"] = trades[i].openTime.toString(Qt::ISODate);
        trade["closeTime"] = trades[i].closeTime.toString(Qt::ISODate);
        trade["closePrice"] = trades[i].closePrice;
        trade["status"] = (trades[i].status == TradeStatus::OPEN) ? "OPEN" : "CLOSED";
        trade["profitLoss"] = trades[i].profitLoss;

        array.append(trade);
    }

    obj["trades"] = array;

    QJsonDocument doc(obj);
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(doc.toJson());
    file.close();
}

void TradingSimulator::loadFromFile(QString filename)
{
    if(!QFile::exists(filename)) {
        return;
    }

    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonObject obj = doc.object();

    balance = obj["balance"].toDouble();
    nextTradeId = obj["nextTradeId"].toInt();

    QJsonArray arr = obj["trades"].toArray();

    for(const QJsonValue& val : arr) {
        QJsonObject tradeObj = val.toObject();

        TradeData trade;
        trade.id = tradeObj["id"].toInt();
        trade.ticker = tradeObj["ticker"].toString();
        trade.type = (tradeObj["type"].toString() == "BUY") ? TradeType::BUY : TradeType::SELL;
        trade.entryPrice = tradeObj["entryPrice"].toDouble();
        trade.amount = tradeObj["amount"].toDouble();
        trade.quantity = tradeObj["quantity"].toDouble();
        trade.takeProfit = tradeObj["takeProfit"].toDouble();
        trade.stopLoss = tradeObj["stopLoss"].toDouble();
        trade.openTime = QDateTime::fromString(tradeObj["openTime"].toString(), Qt::ISODate);
        trade.closeTime = QDateTime::fromString(tradeObj["closeTime"].toString(), Qt::ISODate);
        trade.closePrice = tradeObj["closePrice"].toDouble();
        trade.status = (tradeObj["status"].toString() == "OPEN") ? TradeStatus::OPEN : TradeStatus::CLOSED;
        trade.profitLoss = tradeObj["profitLoss"].toDouble();

        trades.append(trade);

    }

}
