#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QDateTimeAxis>
#include <QDateTime>


#include "includes/CandleData.h"
#include "includes/CustomChartView.h"
#include "includes/TradeData.h"

class QBarCategoryAxis;
class QValueAxis;
class QChart;
class QChartView;
class QCandlestickSeries;
class QLineSeries;
class QBarSeries;
class QScatterSeries;

struct TradeMarker {
    qint64 timestamp;
    double price;
    TradeType type;
};

class ChartWidget : public QWidget
{
    Q_OBJECT
public:
    ChartWidget(QWidget*);
    void setCandles(QList<CandleData>);
    void updateLastCandle(CandleData);
    void createTradeMarker(qint64 timestamp, double price, TradeType type);
    void createTakeProfitLine(double price, bool add = true);
    void createStopLossLine(double price, bool add = true);
    void setShowMA(bool show);
    void setShowRSI(bool show);
    void setShowMACD(bool show);
    void setMAPeriod(int period);
    void setRSIPeriod(int period);
    ~ChartWidget();
private:
    qreal factor;
    QPoint lastMouse;
    bool isDragging;
    int currentInterval = 1;
    int maPeriod = 20;
    int rsiPeriod = 14;
    QList<TradeMarker> tradeMarkers;

    QChart* mainChart;
    QChart* rsiChart;
    QChart* macdChart;

    CustomChartView* mainChartView;
    CustomChartView* rsiChartView;
    CustomChartView* macdChartView;

    QLineSeries* rsiSeries;
    QLineSeries* rsiUpperLine;  // Линия 70
    QLineSeries* rsiLowerLine;  // Линия 30
    QLineSeries* macdLine;
    QLineSeries* signalLine;
    QLineSeries* macdZeroLine;
    QScatterSeries* buyMarkers;
    QScatterSeries* sellMarkers;
    QList<QLineSeries*> takeProfitSeries;
    QList<QLineSeries*> stopLossSeries;

    QCandlestickSeries* series;
    QLineSeries* maSeries;

    QDateTimeAxis* axisx;
    QValueAxis* axisY;
    QDateTimeAxis* rsiAxisX;
    QValueAxis* rsiAxisY;
    QDateTimeAxis* macdAxisX;
    QValueAxis* macdAxisY;
protected:
    void wheelEvent(QWheelEvent* event) override;
signals:
    void axisXRangeChanged(QDateTime min, QDateTime max);
    void indicatorsUpdated(double currentPrice, double maValue, double rsiValue, double macdLine, double signalLine);
    void intervalChanged(int interval);
    void lastCandleTimestamp(qint64 timestamp);

};

#endif // CHARTWIDGET_H
