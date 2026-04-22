#ifndef BYBITAPI_H
#define BYBITAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include "TickerData.h"
#include "CandleData.h"

class BybitApi : public QObject
{
    Q_OBJECT
public:
    BybitApi(QObject*);
    void fetchPrice(QString);
    void fetchCandle(QString, int, int);

private:
    QNetworkAccessManager* manager;
    QNetworkAccessManager* candleManager;
signals:
    void priceReceived(TickerData);
    void candleReceived(QList<CandleData>);
    void error(QString);
    void noErrors();
private slots:
    void onReplyFinished(QNetworkReply*);

};

#endif // BYBITAPI_H
