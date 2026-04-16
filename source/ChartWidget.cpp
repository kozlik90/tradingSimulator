#include "ChartWidget.h"
#include "qdatetime.h"
#include <QChart>
#include <QChartView>
#include <QCandlestickSeries>
#include <QcandlestickSet>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QLineSeries>
#include <QBarCategoryAxis>
#include <QLineSeries>
#include <QBarSeries>
#include <QBarSet>
#include <QScatterSeries>
#include <QComboBox>
#include "TechnicalIndicators.h"
#include "CustomScrollArea.h"

ChartWidget::ChartWidget(QWidget* p) : QWidget(p) {
    factor = 1.0;
    isDragging = false;
    series = new QCandlestickSeries();
    series->setIncreasingColor(QColor(Qt::green));
    series->setDecreasingColor(QColor(Qt::red));
    QPen pen = series->pen();
    pen.setWidth(1);
    series->setPen(pen);

    mainChart = new QChart();
    mainChart->addSeries(series);
    mainChart->setTitle("Price");
    mainChart->legend()->hide();

    axisx = new QDateTimeAxis();
    axisx->setFormat("hh:mm");
    axisx->setTickCount(10);
    axisY = new QValueAxis();
    mainChart->addAxis(axisx, Qt::AlignBottom);
    series->attachAxis(axisx);
    mainChart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    maSeries = new QLineSeries(this);
    maSeries->setName("MA");
    maSeries->setColor(Qt::blue);
    pen = maSeries->pen();
    pen.setWidth(2);
    maSeries->setPen(pen);

    mainChart->addSeries(maSeries);

    maSeries->attachAxis(axisx);
    maSeries->attachAxis(axisY);

    buyMarkers = new QScatterSeries(this);
    buyMarkers->setName("Buy");
    buyMarkers->setColor(Qt::green);
    buyMarkers->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    buyMarkers->setMarkerSize(10);
    pen = buyMarkers->pen();
    pen.setColor(Qt::black);
    pen.setWidth(1);
    buyMarkers->setPen(pen);

    mainChart->addSeries(buyMarkers);

    buyMarkers->attachAxis(axisx);
    buyMarkers->attachAxis(axisY);

    sellMarkers = new QScatterSeries(this);
    sellMarkers->setName("Sell");
    sellMarkers->setColor(Qt::red);
    sellMarkers->setMarkerShape(QScatterSeries::MarkerShapeCircle);
    sellMarkers->setMarkerSize(10);
    pen = sellMarkers->pen();
    pen.setColor(Qt::black);
    pen.setWidth(1);
    sellMarkers->setPen(pen);

    mainChart->addSeries(sellMarkers);

    sellMarkers->attachAxis(axisx);
    sellMarkers->attachAxis(axisY);

    // takeProfitSeries = new QLineSeries(this);
    // takeProfitSeries->setName("TakeProfit");
    // takeProfitSeries->setColor(Qt::green);
    // pen = takeProfitSeries->pen();
    // pen.setStyle(Qt::DashLine);
    // pen.setWidth(1);
    // takeProfitSeries->setPen(pen);

    // mainChart->addSeries(takeProfitSeries);

    // takeProfitSeries->attachAxis(axisx);
    // takeProfitSeries->attachAxis(axisY);

    // stopLossSeries = new QLineSeries(this);
    // stopLossSeries->setName("StopLoss");
    // stopLossSeries->setColor(Qt::red);
    // pen = stopLossSeries->pen();
    // pen.setStyle(Qt::DashLine);
    // pen.setWidth(1);
    // stopLossSeries->setPen(pen);

    // mainChart->addSeries(stopLossSeries);

    //stopLossSeries->attachAxis(axisx);
    //stopLossSeries->attachAxis(axisY);

    mainChartView = new CustomChartView(mainChart, this);

    rsiSeries = new QLineSeries(this);
    rsiSeries->setName("RSI");
    rsiSeries->setColor(Qt::black);
    pen = rsiSeries->pen();
    pen.setWidth(2);
    rsiSeries->setPen(pen);

    rsiChart = new QChart();
    rsiChart->addSeries(rsiSeries);

    rsiAxisX = new QDateTimeAxis();
    rsiAxisX->setFormat("hh:mm");
    rsiAxisX->setTickCount(10);
    rsiAxisY = new QValueAxis();
    rsiAxisY->setRange(0, 100);
    rsiChart->addAxis(rsiAxisX, Qt::AlignBottom);
    rsiSeries->attachAxis(rsiAxisX);
    rsiChart->addAxis(rsiAxisY, Qt::AlignLeft);
    rsiSeries->attachAxis(rsiAxisY);

    // Линия перекупленности (70)
    rsiUpperLine = new QLineSeries(this);
    rsiUpperLine->setName("Overbought");
    rsiUpperLine->setColor(Qt::red);
    QPen upperPen = rsiUpperLine->pen();
    upperPen.setStyle(Qt::DashLine);  // Пунктирная линия
    rsiUpperLine->setPen(upperPen);

    rsiChart->addSeries(rsiUpperLine);

    rsiUpperLine->attachAxis(rsiAxisX);
    rsiUpperLine->attachAxis(rsiAxisY);

    // Линия перепроданности (30)
    rsiLowerLine = new QLineSeries(this);
    rsiLowerLine->setName("Oversold");
    rsiLowerLine->setColor(Qt::green);
    QPen lowerPen = rsiLowerLine->pen();
    lowerPen.setStyle(Qt::DashLine);
    rsiLowerLine->setPen(lowerPen);

    rsiChart->addSeries(rsiLowerLine);

    rsiLowerLine->attachAxis(rsiAxisX);
    rsiLowerLine->attachAxis(rsiAxisY);

    rsiChartView = new CustomChartView(rsiChart, this);
    rsiChartView->setVerticalScrollEnabled(false);

    connect(mainChartView, &CustomChartView::axisXRangeChanged, this, [this](QDateTime min, QDateTime max){
        rsiAxisX->setRange(min, max);
        macdAxisX->setRange(min, max);
    });
    connect(this, &ChartWidget::axisXRangeChanged, this, [this](QDateTime min, QDateTime max) {
        rsiAxisX->setRange(min, max);
        macdAxisX->setRange(min, max);
    });

    macdLine = new QLineSeries(this);
    macdLine->setName("MACD");
    macdLine->setColor(Qt::darkYellow);
    pen = macdLine->pen();
    pen.setWidth(2);
    macdLine->setPen(pen);

    signalLine = new QLineSeries(this);
    signalLine->setName("Signal");
    signalLine->setColor(Qt::darkBlue);
    pen = signalLine->pen();
    pen.setWidth(2);
    signalLine->setPen(pen);

    macdChart = new QChart();
    macdChart->addSeries(macdLine);
    macdChart->addSeries(signalLine);

    macdAxisX = new QDateTimeAxis();
    macdAxisX->setFormat("hh:mm");
    macdAxisX->setTickCount(10);
    macdAxisY = new QValueAxis();
    macdChart->addAxis(macdAxisX, Qt::AlignBottom);
    macdLine->attachAxis(macdAxisX);
    signalLine->attachAxis(macdAxisX);
    macdChart->addAxis(macdAxisY, Qt::AlignLeft);
    macdLine->attachAxis(macdAxisY);
    signalLine->attachAxis(macdAxisY);

    macdZeroLine = new QLineSeries(this);
    macdZeroLine->setName("Zero");
    macdZeroLine->setColor(Qt::black);
    QPen zeroPen = macdZeroLine->pen();
    zeroPen.setStyle(Qt::DashLine);
    macdZeroLine->setPen(zeroPen);

    macdChart->addSeries(macdZeroLine);

    macdZeroLine->attachAxis(macdAxisX);
    macdZeroLine->attachAxis(macdAxisY);

    macdChartView = new CustomChartView(macdChart, this);
    macdChartView->setVerticalScrollEnabled(false);

    QWidget* container = new QWidget();
    QVBoxLayout* chartsLayout = new QVBoxLayout(container);
    chartsLayout->setContentsMargins(0, 0, 0, 0);
    chartsLayout->setSpacing(2);

    // Установи фиксированную высоту для каждого графика
    mainChartView->setMinimumHeight(600);  // Нормальная высота
    rsiChartView->setMinimumHeight(300);
    macdChartView->setMinimumHeight(300);

    chartsLayout->addWidget(mainChartView);
    chartsLayout->addWidget(rsiChartView);
    chartsLayout->addWidget(macdChartView);

    // Оберни в QScrollArea
    CustomScrollArea* scrollArea = new CustomScrollArea(this);
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);  // Важно!
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // Только вертикальная прокрутка

    // Добавь scrollArea в главный layout
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(scrollArea);

    QComboBox* cmbInterval = new QComboBox(mainChartView);
    cmbInterval->addItem("1m", 1);
    cmbInterval->addItem("5m", 5);
    cmbInterval->addItem("15m", 15);
    cmbInterval->addItem("30m", 30);
    cmbInterval->addItem("1h", 60);
    cmbInterval->addItem("4h", 240);
    cmbInterval->addItem("1D", 1440);

    cmbInterval->setFixedWidth(80);

    cmbInterval->move(10, 10);

    cmbInterval->raise();

    cmbInterval->setStyleSheet("QComboBox { background-color: rgba(255, 255, 255, 200); }");

    connect(cmbInterval, &QComboBox::currentIndexChanged,
            this, [this, cmbInterval](int index){
                int interval = cmbInterval->itemData(index).toInt();
                currentInterval = interval;
                emit intervalChanged(interval);
                axisx->setFormat("hh:mm");
                mainChartView->setAxisXFormat("hh:mm");
                if(interval == 60 || interval == 240) {
                    axisx->setFormat("hh:00");
                    mainChartView->setAxisXFormat("hh:00");
                }
                if(interval == 1440) {
                    axisx->setFormat("dd.MM");
                    mainChartView->setAxisXFormat("dd.MM");
                }
            });
}
void ChartWidget::setCandles(QList<CandleData> list)
{
    factor = 1.0;
    series->clear();
    maSeries->clear();
    rsiSeries->clear();
    macdLine->clear();
    signalLine->clear();
    //takeProfitSeries->clear();
    //stopLossSeries->clear();

    double max = 0;
    double min = 999999999;
    if (!list.isEmpty()) {
        for(int i = 0; i < list.size(); ++i) {
            QCandlestickSet* set = new QCandlestickSet();
            set->setOpen(list[i].openPrice);
            set->setHigh(list[i].highPrice);
            set->setLow(list[i].lowPrice);
            set->setClose(list[i].closePrice);
            set->setTimestamp(list[i].timestamp);

            if(set) {
                series->append(set);
            }

        }
        emit lastCandleTimestamp(list.last().timestamp);

        if (mainChart->axes(Qt::Horizontal).isEmpty()) {
            mainChart->addAxis(axisx, Qt::AlignBottom);
            series->attachAxis(axisx);
        }

        QDateTime minTime = QDateTime::fromMSecsSinceEpoch(list.first().timestamp);
        QDateTime maxTime = QDateTime::fromMSecsSinceEpoch(list.last().timestamp);

        int visibleCandles = qMin(100, list.size());  // Не больше чем есть
        QDateTime visibleStartTime = QDateTime::fromMSecsSinceEpoch(list[list.size() - visibleCandles].timestamp);

        axisx->setRange(visibleStartTime, maxTime);
        for(int i = list.size()-visibleCandles; i < list.size(); ++i) {
            list[i].highPrice > max ? max = list[i].highPrice : max;
            list[i].lowPrice < min ? min = list[i].lowPrice : min;
        }


        rsiAxisX = qobject_cast<QDateTimeAxis *>(rsiChart->axes(Qt::Horizontal).first());
        rsiAxisY =qobject_cast<QValueAxis *>(rsiChart->axes(Qt::Vertical).first());

        if (rsiAxisX && rsiAxisY) {
            rsiAxisX->setRange(visibleStartTime, maxTime);
            rsiAxisY->setRange(0, 100);
        }

        QList<double> ma = TechnicalIndicators::calculateMA(list, maPeriod);

        for (int i = 0; i < ma.size(); ++i) {
            maSeries->append(list[i].timestamp, ma[i]);
        }


        QList<double> rsi = TechnicalIndicators::calculateRSI(list, rsiPeriod);

        for (int i = 0; i < rsi.size(); ++i) {
            rsiSeries->append(list[i + rsiPeriod].timestamp, rsi[i]);
        }

        rsiUpperLine->clear();
        rsiUpperLine->append(minTime.toMSecsSinceEpoch(), 70);
        rsiUpperLine->append(maxTime.toMSecsSinceEpoch(), 70);



        rsiLowerLine->clear();
        rsiLowerLine->append(minTime.toMSecsSinceEpoch(), 30);
        rsiLowerLine->append(maxTime.toMSecsSinceEpoch(), 30);


        int slowPeriod = 26, fastPeriod = 12, signalPeriod = 9;
        QList<MACDData> macd = TechnicalIndicators::calculateMACD(
            list, fastPeriod, slowPeriod, signalPeriod);

        int signalStartIndex = slowPeriod + signalPeriod - 2;

        if (!macd.isEmpty()) {
            for (int i = 0; i < macd.size(); ++i) {
                int candleIndex = signalStartIndex + i;

                if (candleIndex < list.size()) {
                    macdLine->append(list[candleIndex].timestamp, macd[i].macdLine);
                    signalLine->append(list[candleIndex].timestamp, macd[i].signal);
                }
            }
            double maxAbs = 0;
            for (const auto &data : macd) {
                maxAbs = qMax(maxAbs, qAbs(data.macdLine));
                maxAbs = qMax(maxAbs, qAbs(data.signal));
                maxAbs = qMax(maxAbs, qAbs(data.histogram));
            }

            macdAxisX = qobject_cast<QDateTimeAxis *>(
                macdChart->axes(Qt::Horizontal).first());
            macdAxisY =
                qobject_cast<QValueAxis *>(macdChart->axes(Qt::Vertical).first());
            if (macdAxisX && macdAxisY) {
                macdAxisX->setRange(visibleStartTime, maxTime);
                macdAxisY->setRange(-maxAbs * 1.1, maxAbs * 1.1);
            }
        }
        macdZeroLine->clear();
        macdZeroLine->append(minTime.toMSecsSinceEpoch(), 0.0);
        macdZeroLine->append(maxTime.toMSecsSinceEpoch(), 0.0);


        if(!ma.isEmpty() && !rsi.isEmpty() && !macd.isEmpty()) {
            emit indicatorsUpdated(list.last().closePrice, ma.last(), rsi.last(), macd.last().macdLine, macd.last().signal);
        }

        buyMarkers->clear();
        sellMarkers->clear();
        tradeMarkers.clear();

    }

    axisY->setMax(max * 1.001);
    axisY->setMin(min * 0.999);


}

void ChartWidget::updateLastCandle(CandleData candle)
{

    if(!series->sets().isEmpty()) {
        QCandlestickSet* lastCandle = series->sets().last();
        qint64 lastTimestamp = static_cast<qint64>(lastCandle->timestamp());

        if(candle.timestamp == lastTimestamp) {
            lastCandle->setOpen(candle.openPrice);
            lastCandle->setHigh(candle.highPrice);
            lastCandle->setLow(candle.lowPrice);
            lastCandle->setClose(candle.closePrice);
            lastCandle->setTimestamp(candle.timestamp);
            QList<CandleData> allCandles;
            for (auto* set : series->sets()) {
                CandleData c;
                c.timestamp = set->timestamp();
                c.openPrice = set->open();
                c.highPrice = set->high();
                c.lowPrice = set->low();
                c.closePrice = set->close();
                allCandles.append(c);
            }
            QList<double> ma = TechnicalIndicators::calculateMA(allCandles, maPeriod);
            QPointF point = maSeries->points().last();
            maSeries->remove(point);
            maSeries->append(allCandles.last().timestamp, ma.last());

            qint64 currentTimestamp = allCandles.last().timestamp;
            if(!buyMarkers->points().isEmpty()) {
                QPointF markerPoint = buyMarkers->points().last();
                if(currentTimestamp == static_cast<qint64>(markerPoint.x())) {
                    buyMarkers->remove(markerPoint);
                    buyMarkers->append(allCandles.last().timestamp, allCandles.last().highPrice);
                }
            }
            if(!sellMarkers->points().isEmpty()) {
                QPointF markerPoint = sellMarkers->points().last();
                if(currentTimestamp == static_cast<qint64>(markerPoint.x())) {
                    sellMarkers->remove(markerPoint);
                    sellMarkers->append(allCandles.last().timestamp, allCandles.last().highPrice);
                }
            }

            QList<double> rsi = TechnicalIndicators::calculateRSI(allCandles, rsiPeriod);

            if (!rsi.isEmpty() && !rsiSeries->points().isEmpty()) {
                QPointF lastPoint = rsiSeries->points().last();
                rsiSeries->remove(lastPoint);
                rsiSeries->append(allCandles.last().timestamp, rsi.last());
            }

            QList<MACDData> macd = TechnicalIndicators::calculateMACD(allCandles);
            if(!macd.isEmpty() && !macdLine->points().isEmpty() && !signalLine->points().isEmpty()) {
                QPointF lastMacd = macdLine->points().last();
                QPointF lastSignal = signalLine->points().last();
                macdLine->remove(lastMacd);
                signalLine->remove(lastSignal);
                macdLine->append(allCandles.last().timestamp, macd.last().macdLine);
                signalLine->append(allCandles.last().timestamp, macd.last().signal);
            }
            emit indicatorsUpdated(allCandles.last().closePrice, ma.last(), rsi.last(), macd.last().macdLine, macd.last().signal);
        }
        else {
            QCandlestickSet* set = new QCandlestickSet();
            set->setOpen(candle.openPrice);
            set->setHigh(candle.highPrice);
            set->setLow(candle.lowPrice);
            set->setClose(candle.closePrice);
            set->setTimestamp(candle.timestamp);
            series->append(set);
            if(series->count() > 1000) {
                series->remove(series->sets().first());
                maSeries->remove(maSeries->points().first());
                rsiSeries->remove(rsiSeries->points().first());
                rsiUpperLine->remove(rsiUpperLine->points().first());
                rsiLowerLine->remove(rsiLowerLine->points().first());
                macdLine->remove(macdLine->points().first());
                signalLine->remove(signalLine->points().first());
                macdZeroLine->remove(macdZeroLine->points().first());
                for(auto& item : takeProfitSeries) {
                    item->remove(item->points().first());
                }
                for(auto& item : stopLossSeries) {
                    item->remove(item->points().first());
                }
            }
            QList<CandleData> allCandles;
            for (auto* set : series->sets()) {
                CandleData c;
                c.timestamp = set->timestamp();
                c.openPrice = set->open();
                c.highPrice = set->high();
                c.lowPrice = set->low();
                c.closePrice = set->close();
                allCandles.append(c);
            }
            emit lastCandleTimestamp(allCandles.last().timestamp);
            QList<double> ma20 = TechnicalIndicators::calculateMA(allCandles, maPeriod);
            maSeries->append(allCandles.last().timestamp, ma20.last());

            QList<double> rsi = TechnicalIndicators::calculateRSI(allCandles, rsiPeriod);

            if (!rsi.isEmpty()) {
                rsiSeries->append(allCandles.last().timestamp, rsi.last());
            }

            QList<MACDData> macd = TechnicalIndicators::calculateMACD(allCandles);
            if(!macd.isEmpty()) {
                macdLine->append(allCandles.last().timestamp, macd.last().macdLine);
                signalLine->append(allCandles.last().timestamp, macd.last().signal);
            }

            QDateTime minTime = QDateTime::fromMSecsSinceEpoch(allCandles.first().timestamp);
            QDateTime maxTime = QDateTime::fromMSecsSinceEpoch(allCandles.last().timestamp);
            rsiUpperLine->append(maxTime.toMSecsSinceEpoch(), 70);
            rsiLowerLine->append(maxTime.toMSecsSinceEpoch(), 30);
            macdZeroLine->append(maxTime.toMSecsSinceEpoch(), 0.0);
            for(auto& item : takeProfitSeries) {
                item->append(maxTime.toMSecsSinceEpoch(), item->points().last().y());
            }
            for(auto& item : stopLossSeries) {
                item->append(maxTime.toMSecsSinceEpoch(), item->points().last().y());
            }
            emit indicatorsUpdated(allCandles.last().closePrice, ma20.last(), rsi.last(), macd.last().macdLine, macd.last().signal);

        }

    }


}

void ChartWidget::createTradeMarker(qint64 timestamp, double price, TradeType type)
{

    if(series->sets().isEmpty()) return;

    // Найди ближайшую свечу
    QCandlestickSet* closestSet = nullptr;
    qint64 minDiff = LLONG_MAX;

    for (auto* set : series->sets()) {
        qint64 diff = qAbs(timestamp - static_cast<qint64>(set->timestamp()));
        if(diff < minDiff) {
            minDiff = diff;
            closestSet = set;
        }
    }

    if(closestSet) {
        qint64 candleTimestamp = static_cast<qint64>(closestSet->timestamp());
        double markerPrice = closestSet->high();

        if(type == TradeType::BUY) {
            buyMarkers->append(candleTimestamp, markerPrice);
        } else {
            sellMarkers->append(candleTimestamp, markerPrice);
        }
        tradeMarkers.append({timestamp, price, type});
    }

}

void ChartWidget::createTakeProfitLine(double price, bool add)
{

    QList<CandleData> allCandles;
    for (auto* set : series->sets()) {
        CandleData c;
        c.timestamp = set->timestamp();
        c.openPrice = set->open();
        c.highPrice = set->high();
        c.lowPrice = set->low();
        c.closePrice = set->close();
        allCandles.append(c);
    }
    QLineSeries* series = new QLineSeries(this);
    if(add) {
        series->setName("TakeProfit");
        series->setColor(Qt::green);
        QPen pen = series->pen();
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        series->setPen(pen);

        mainChart->addSeries(series);

        series->attachAxis(axisx);
        series->attachAxis(axisY);

        series->append(allCandles.first().timestamp, price);
        series->append(allCandles.last().timestamp, price);
        takeProfitSeries.append(series);

    }
    else {
        for(auto& item : takeProfitSeries) {
            if(item->points().last().y() == price){
                item->clear();
                delete item;
                takeProfitSeries.removeOne(item);
                break;
            }
        }
    }

}

void ChartWidget::createStopLossLine(double price, bool add)
{
    QList<CandleData> allCandles;
    for (auto* set : series->sets()) {
        CandleData c;
        c.timestamp = set->timestamp();
        c.openPrice = set->open();
        c.highPrice = set->high();
        c.lowPrice = set->low();
        c.closePrice = set->close();
        allCandles.append(c);
    }
    QLineSeries* series = new QLineSeries(this);
    if(add) {
        series->setName("StopLoss");
        series->setColor(Qt::red);
        QPen pen = series->pen();
        pen.setStyle(Qt::DashLine);
        pen.setWidth(1);
        series->setPen(pen);

        mainChart->addSeries(series);

        series->attachAxis(axisx);
        series->attachAxis(axisY);

        series->append(allCandles.first().timestamp, price);
        series->append(allCandles.last().timestamp, price);
        stopLossSeries.append(series);

    }
    else {
        for(auto& item : stopLossSeries) {
            if(item->points().last().y() == price){
                item->clear();
                delete item;
                stopLossSeries.removeOne(item);
                break;

            }
        }
    }
}

void ChartWidget::setShowMA(bool show)
{
    maSeries->setVisible(show);
}

void ChartWidget::setShowRSI(bool show)
{
    rsiChartView->setVisible(show);
}

void ChartWidget::setShowMACD(bool show)
{
    macdChartView->setVisible(show);
}

void ChartWidget::setMAPeriod(int period)
{
    maSeries->clear();
    maPeriod = period;
    QList<CandleData> allCandles;
    for (auto* set : series->sets()) {
        CandleData c;
        c.timestamp = set->timestamp();
        c.openPrice = set->open();
        c.highPrice = set->high();
        c.lowPrice = set->low();
        c.closePrice = set->close();
        allCandles.append(c);
    }
    QList<double> ma = TechnicalIndicators::calculateMA(allCandles, maPeriod);
    for (int i = 0; i < ma.size(); ++i) {
        maSeries->append(allCandles[i].timestamp, ma[i]);
    }

}

void ChartWidget::setRSIPeriod(int period)
{
    rsiSeries->clear();
    rsiPeriod = period;

    QList<CandleData> allCandles;
    for (auto* set : series->sets()) {
        CandleData c;
        c.timestamp = set->timestamp();
        c.openPrice = set->open();
        c.highPrice = set->high();
        c.lowPrice = set->low();
        c.closePrice = set->close();
        allCandles.append(c);
    }

    QList<double> rsi = TechnicalIndicators::calculateRSI(allCandles, rsiPeriod);

    for (int i = 0; i < rsi.size(); ++i) {
        rsiSeries->append(allCandles[i + period].timestamp, rsi[i]);
    }
}

ChartWidget::~ChartWidget()
{
    qDeleteAll(takeProfitSeries);
    takeProfitSeries.clear();
    qDeleteAll(stopLossSeries);
    stopLossSeries.clear();
}

void ChartWidget::wheelEvent(QWheelEvent *event)
{
    bool ctrlPressed = event->modifiers() & Qt::ControlModifier;
    if (ctrlPressed) {
        // Zoom только по оси Y
        if(event->angleDelta().y() > 0) {
            // Приблизить по Y
            qreal currentRange = axisY->max() - axisY->min();
            qreal center = (axisY->max() + axisY->min()) / 2;
            qreal newRange = currentRange * 0.95;  // Уменьшить диапазон

            axisY->setRange(center - newRange/2, center + newRange/2);

        } else {
            // Отдалить по Y
            qreal currentRange = axisY->max() - axisY->min();
            qreal center = (axisY->max() + axisY->min()) / 2;
            qreal newRange = currentRange * 1.05;  // Увеличить диапазон

            axisY->setRange(center - newRange/2, center + newRange/2);
        }
    }
    else {
        if(event->angleDelta().y() > 0) {
            factor *= 1.05;
            mainChart->zoom(1.05f);

        }
        else {
            if(factor > 0.4) {
                qDebug() <<factor;
                factor *= 0.95;
                mainChart->zoom(0.95);
            }

        }
        QDateTime xMax = axisx->max();
        QDateTime xMin = axisx->min();
        emit axisXRangeChanged(xMin, xMax);

    }
}



