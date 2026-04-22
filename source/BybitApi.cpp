#include "includes/BybitApi.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

BybitApi::BybitApi(QObject* p) : QObject(p) {
    manager = new QNetworkAccessManager(this);
    candleManager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
    connect(candleManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onReplyFinished(QNetworkReply*)));
}

void BybitApi::fetchPrice(QString ticker)
{
    QString url = QString("https://api.bybit.com/v5/market/tickers?category=spot&symbol=%1").arg(ticker);
    QNetworkRequest request((QUrl)url);
    request.setAttribute(QNetworkRequest::User, ticker);
    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User+1), "price");
    manager->get(request);

}

void BybitApi::fetchCandle(QString symbol, int interval, int limit)
{
    QString inter = QString::number(interval);
    if(interval == 1440) {
        inter = "D";
    }
    QString url = QString("https://api.bybit.com/v5/market/kline?category=spot&symbol=%1&interval=%2&limit=%3").arg(symbol).arg(inter).
                  arg(QString::number(limit));
    QNetworkRequest request((QUrl)url);
    request.setAttribute(QNetworkRequest::User, symbol);
    request.setAttribute(QNetworkRequest::Attribute(QNetworkRequest::User+1), "candle");
    QNetworkReply* reply = candleManager->get(request);

    connect(reply, &QNetworkReply::errorOccurred, [=](){
       // qDebug() <<reply->error();
        //qDebug() << "Error occurred:" << reply->errorString();
    });


}


void BybitApi::onReplyFinished(QNetworkReply *reply)
{
    if(reply->error()) {
        emit error(reply->errorString());
        return;
    }

    QString type = reply->request().attribute(QNetworkRequest::Attribute(QNetworkRequest::User+1)).toString();
    QString coinName = reply->request().attribute(QNetworkRequest::User).toString();
    QByteArray data = reply->readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj(doc.object());

    QJsonValue result = obj["result"];
    QJsonValue list = result["list"];
    if(type == "price") {
        QString price = list[0]["lastPrice"].toString();
        TickerData tickerData;
        tickerData.ticker = coinName;
        tickerData.price = price.toDouble();
        emit priceReceived(tickerData);
    }
    else if(type == "candle") {
        QJsonArray array = list.toArray();
        QList<CandleData> candles;
        for(int i = 0; i < array.size(); ++i) {
            QJsonArray candleResp = array[i].toArray();
            CandleData candle;
            candle.timestamp = candleResp[0].toString().toLongLong();
            candle.openPrice = candleResp[1].toString().toDouble();
            candle.highPrice = candleResp[2].toString().toDouble();
            candle.lowPrice = candleResp[3].toString().toDouble();
            candle.closePrice = candleResp[4].toString().toDouble();
            candle.volume = candleResp[5].toString().toDouble();
            candles.push_front(candle); //добавляем в начало, т.к. i=[0] - самая свежая свеча, i= array.size - самая старая свеча
        }
        emit candleReceived(candles);
    }
    emit noErrors();



}

