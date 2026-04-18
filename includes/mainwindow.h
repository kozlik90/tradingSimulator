#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CoinData.h"
#include "BybitApi.h"
#include "ChartWidget.h"
#include "TradingSimulator.h"
#include "SettingsDialog.h"
#include "MainSettings.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    BybitApi* api;
    TradingSimulator* simulator;

    QList<CoinData*> coinList;
    QString currentTicker;
    qint64 currentCandleTimestamp;
    QTimer* timer;
    QTimer* candleTimer;
    int currentInterval = 1;
private slots:

    void onPriceChanged(QString, double);
    void onPriceReceived(TickerData);
    void onCoinSelected(int, int);
    void onCandleReceived(QList<CandleData>);
    void fetchPrice();
    void updateCurrentCandle();
    void onIndicatorsUpdated(double price, double ma, double rsi, double macd, double signal);
    void onIntervalChanged(int interval);

    void on_btn_LONG_clicked();
    void on_btn_Short_clicked();
    void onTradeOpened(TradeData trade);
    void onTradeClosed(TradeData trade);
    void onProfitLossUpdated(int tradeId, double profitLoss);
    void onPositionDoubleClicked(int row, int column);
    void updateUIFromSimulator();
    void closeEvent(QCloseEvent* event) override;
    void on_actionGraphSettings_triggered();
    void on_actionExit_triggered();
    void on_action_2_triggered();
};
#endif // MAINWINDOW_H
