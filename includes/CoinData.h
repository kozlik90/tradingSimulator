#ifndef COINDATA_H
#define COINDATA_H

#include "qdatetime.h"
#include <QObject>

class CoinData : public QObject
{
    Q_OBJECT
public:
    CoinData(QString, double, QObject*);
    void setPrice(double);
    double getPrice() {return price; };
    QString getTicker() {return ticker; };

private:
    QString ticker;
    double price;
    QDateTime lastTime;
signals:
    void priceChanged(QString, double);
};

#endif // COINDATA_H
