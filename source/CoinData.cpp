#include "CoinData.h"


CoinData::CoinData(QString t, double p, QObject* parrent = nullptr) : QObject(parrent), ticker(t), price(p)
{
    lastTime = QDateTime::currentDateTime();
}

void CoinData::setPrice(double p) {
    if(price != p){
        price = p;
        lastTime = QDateTime::currentDateTime();
        emit priceChanged(ticker, p);
    }
}
