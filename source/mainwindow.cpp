#include "includes/mainwindow.h"
#include "ui_mainwindow.h"
#include <QHeaderView>
#include <QTimer>
#include <QNetworkReply>
#include <QMessageBox>
#include <QSettings>
#include <QRegularExpressionValidator>
#include "includes/TradingSignals.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    api = new BybitApi(this);
    simulator = new TradingSimulator(1000, this);

    connect(api, &BybitApi::priceReceived, this, &MainWindow::onPriceReceived);
    connect(api, &BybitApi::candleReceived, this, &MainWindow::onCandleReceived);
    connect(ui->tableCoin, &QTableWidget::cellClicked, this, &MainWindow::onCoinSelected);
    connect(ui->chart_widget, &ChartWidget::indicatorsUpdated, this, &MainWindow::onIndicatorsUpdated);
    connect(ui->chart_widget, &ChartWidget::intervalChanged, this, &MainWindow::onIntervalChanged);
    connect(ui->chart_widget, &ChartWidget::lastCandleTimestamp, this, [this](qint64 timestamp) {
        currentCandleTimestamp = timestamp;
    });
    connect(simulator, &TradingSimulator::balanceChanged, this, [this](double bal) {
        ui->label_Balance->setText("Баланс: " + QString::number(bal) + "$");
    });
    connect(simulator, &TradingSimulator::tradeOpened, this, &MainWindow::onTradeOpened);
    connect(simulator, &TradingSimulator::tradeClosed, this, &MainWindow::onTradeClosed);
    connect(simulator, &TradingSimulator::profitLossUpdated, this, &MainWindow::onProfitLossUpdated);
    connect(ui->tablePositions, &QTableWidget::cellDoubleClicked, this, &MainWindow::onPositionDoubleClicked);


    timer = new QTimer(this);
    candleTimer = nullptr;
    connect(timer, SIGNAL(timeout()), this, SLOT(fetchPrice()));
    timer->start(1000);

    ui->tableCoin->setHorizontalHeaderLabels({"Coin", "Price"});
    ui->tableCoin->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableCoin->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tablePositions->setEditTriggers(QAbstractItemView::NoEditTriggers);


    coinList.append(new CoinData("BTCUSDT", 0, this));
    coinList.append(new CoinData("ETHUSDT", 0, this));
    coinList.append(new CoinData("SOLUSDT", 0, this));

    for(auto* coin : coinList) {
        connect(coin, &CoinData::priceChanged, this, &MainWindow::onPriceChanged);
    }

    for(int i = 0; i < coinList.size(); ++i) {
        ui->tableCoin->insertRow(i);
        ui->tableCoin->setItem(i, 0, new QTableWidgetItem(coinList[i]->getTicker()));
    }
    ui->tableCoin->setCurrentCell(0,0);
    onCoinSelected(0, 0);

    fetchPrice();

    simulator->loadFromFile("trades.json");
    updateUIFromSimulator();

    QRegularExpression re("[0-9]{1,9}");
    QRegularExpressionValidator* reValidator = new QRegularExpressionValidator(re, this);
    ui->lineEdit_amount->setValidator(reValidator);
    ui->lineEdit_StopLoss->setValidator(reValidator);
    ui->lineEdit_TakeProfit->setValidator(reValidator);

    QSettings settings("kozlik90 Inc." "user");
    ui->chart_widget->setShowMA(settings.value("chart/showMA", true).toBool());
    ui->chart_widget->setShowRSI(settings.value("chart/showRSI", true).toBool());
    ui->chart_widget->setShowMACD(settings.value("chart/showMACD", true).toBool());
    ui->chart_widget->setMAPeriod(settings.value("chart/maPeriod", 20).toInt());
    ui->chart_widget->setRSIPeriod(settings.value("chart/rsiPeriod", 14).toInt());

}

void MainWindow::fetchPrice()
{
    for(auto* str: coinList) {
        api->fetchPrice(str->getTicker());
    }

}

void MainWindow::updateCurrentCandle()
{
    if(!currentTicker.isEmpty()) {
        api->fetchCandle(currentTicker, currentInterval, 1);
    }
}


void MainWindow::onPriceReceived(TickerData data)
{
    CoinData* coinData = nullptr;
    for(auto* coin : coinList) {
        if(coin->getTicker() == data.ticker) {
            coinData = coin;
            break;
        }
    }
    if (coinData) {
        coinData->setPrice(data.price);
    }
}

void MainWindow::onCandleReceived(QList<CandleData> candles)
{

    if(candles.isEmpty())
        return;
    if(candles.size() == 1) {
        ui->chart_widget->updateLastCandle(candles[0]);
        currentCandleTimestamp = candles[0].timestamp;
    }
    else {
        ui->chart_widget->setCandles(candles);
        QList<TradeData> openTrades = simulator->getOpenTrades();
        for(const TradeData& trade : openTrades) {
            if(trade.ticker == currentTicker) {
                ui->chart_widget->createTradeMarker(trade.openTime.toMSecsSinceEpoch(), trade.entryPrice, trade.type);
                ui->chart_widget->createTakeProfitLine(trade.takeProfit);
                ui->chart_widget->createStopLossLine(trade.stopLoss);
            }
        }
    }
}

void MainWindow::onCoinSelected(int row, int column)
{
    if(currentTicker != ui->tableCoin->item(row, 0)->text()) {
        currentTicker = ui->tableCoin->item(row, 0)->text();
        if(!currentTicker.isEmpty())
            api->fetchCandle(currentTicker, currentInterval, 700);

        if(candleTimer)
            candleTimer->stop();
        if(!candleTimer) {
            candleTimer = new QTimer(this);
            connect(candleTimer, &QTimer::timeout, this, &MainWindow::updateCurrentCandle);
        }
        candleTimer->start(1000);
    }
}

void MainWindow::onIndicatorsUpdated(double price, double ma, double rsi, double macd, double signal)
{
    SignalAnalysis analysis = TradingSignals::analyzeSignals(price, ma, rsi, macd, signal);
    ui->label_Indicators->setText(QString("MA: %1 | RSI: %2\nMACD: %3 | Signal: %4")
                                      .arg(ma)
                                      .arg(rsi)
                                      .arg(macd)
                                      .arg(signal));
    ui->label_Recommendation->setText(QString("Advise: %1").arg(analysis.recommendation));
    if(analysis.recommendation.contains("BUY")) {
        ui->label_Recommendation->setStyleSheet("color: green; font-weight: bold;");
    }
    else if(analysis.recommendation.contains("SELL")) {
        ui->label_Recommendation->setStyleSheet("color: red; font-weight: bold;");
    }
    else {
        ui->label_Recommendation->setStyleSheet("color: black; font-weight: bold;");
    }

}

void MainWindow::onIntervalChanged(int interval)
{
    currentInterval = interval;
    if(!currentTicker.isEmpty()) {
        api->fetchCandle(currentTicker, interval, 700);
    }
}

void MainWindow::onPriceChanged(QString ticker, double price)
{

    for(int i = 0; i < ui->tableCoin->rowCount(); ++i) {
        if(ui->tableCoin->item(i, 0)->text() == ticker) {
            ui->tableCoin->setItem(i, 1, new QTableWidgetItem(QString::number(price)));
            simulator->updateProfitLoss(ticker, price);
        }
        else {
            continue;
        }
    }

}


void MainWindow::on_btn_LONG_clicked()
{
    double amount = ui->lineEdit_amount->text().toDouble();
    double takeProfit = ui->lineEdit_TakeProfit->text().toDouble();
    double stopLoss = ui->lineEdit_StopLoss->text().toDouble();
    double currentPrice = 0.0;
    for(int i = 0; i < coinList.size(); ++i) {
        if(currentTicker == coinList[i]->getTicker()) {
            currentPrice = coinList[i]->getPrice();
        }
    }
    if(amount != 0){
        try {
            simulator->openTrade(currentTicker, TradeType::BUY, amount, currentPrice, takeProfit, stopLoss);
        }
        catch(std::exception& ex) {
            QMessageBox box;
            box.setText(ex.what());
            box.setIcon(QMessageBox::Warning);
            box.exec();
        }
    }

}


void MainWindow::on_btn_Short_clicked()
{
    double amount = ui->lineEdit_amount->text().toDouble();
    double takeProfit = ui->lineEdit_TakeProfit->text().toDouble();
    double stopLoss = ui->lineEdit_StopLoss->text().toDouble();
    double currentPrice = 0.0;
    for(int i = 0; i < coinList.size(); ++i) {
        if(currentTicker == coinList[i]->getTicker()) {
            currentPrice = coinList[i]->getPrice();
        }
    }
    if(amount != 0) {
        try{
            simulator->openTrade(currentTicker, TradeType::SELL, amount, currentPrice, takeProfit, stopLoss);
        }
        catch(std::exception& ex) {
            QMessageBox box;
            box.setText(ex.what());
            box.setIcon(QMessageBox::Warning);
            box.exec();
        }
    }
}

void MainWindow::onTradeOpened(TradeData trade)
{
    int row = ui->tablePositions->rowCount();
    ui->tablePositions->insertRow(row);
    ui->tablePositions->setItem(row, 0, new QTableWidgetItem(QString::number(trade.id)));
    ui->tablePositions->setItem(row, 1, new QTableWidgetItem(trade.ticker));
    if(trade.type == TradeType::BUY) {
        ui->tablePositions->setItem(row, 2, new QTableWidgetItem("LONG"));

    }
    else {
        ui->tablePositions->setItem(row, 2, new QTableWidgetItem("SHORT"));
    }
    ui->tablePositions->setItem(row, 3, new QTableWidgetItem(QString::number(trade.entryPrice, 'f', 2)));
    ui->tablePositions->setItem(row, 4, new QTableWidgetItem(QString::number(trade.amount, 'f', 2)));
    ui->tablePositions->setItem(row, 5, new QTableWidgetItem(QString::number(trade.profitLoss, 'f', 2)));
    ui->tablePositions->setItem(row, 6, new QTableWidgetItem(trade.openTime.toString("dd.MM.yy hh:mm:ss")));
    ui->chart_widget->createTradeMarker(currentCandleTimestamp, trade.entryPrice, trade.type);
    ui->chart_widget->createTakeProfitLine(trade.takeProfit);
    ui->chart_widget->createStopLossLine(trade.stopLoss);
    if(trade.status == TradeStatus::OPEN) {
        ui->tablePositions->setItem(row, 7, new QTableWidgetItem("OPEN"));
    }
    else {
        ui->tablePositions->setItem(row, 7, new QTableWidgetItem("CLOSED"));
    }


}

void MainWindow::onTradeClosed(TradeData trade)
{
    QString status;
    if(trade.status == TradeStatus::CLOSED) {
        status = "CLOSED";
    }
    for(int i = 0; i < ui->tablePositions->rowCount(); ++i) {
        if(trade.id == ui->tablePositions->item(i, 0)->text().toInt()) {
            int rowHistory = ui->tableHistory->rowCount();
            ui->tableHistory->insertRow(rowHistory);
            ui->tableHistory->setItem(rowHistory, 0, new QTableWidgetItem(QString::number(trade.id)));
            ui->tableHistory->setItem(rowHistory, 1, new QTableWidgetItem(trade.ticker));
            if(trade.type == TradeType::BUY) {
                ui->tableHistory->setItem(rowHistory, 2, new QTableWidgetItem("LONG"));

            }
            else {
                ui->tableHistory->setItem(rowHistory, 2, new QTableWidgetItem("SHORT"));
            }
            ui->tableHistory->setItem(rowHistory, 3, new QTableWidgetItem(QString::number(trade.entryPrice, 'f', 2)));
            ui->tableHistory->setItem(rowHistory, 4, new QTableWidgetItem(QString::number(trade.amount, 'f', 2)));
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(trade.profitLoss, 'f', 2));
            if(trade.profitLoss >= 0) item->setForeground(QBrush(Qt::green));
            else item->setForeground(QBrush(Qt::red));
            ui->tableHistory->setItem(rowHistory, 5, item);
            ui->tableHistory->setItem(rowHistory, 6, new QTableWidgetItem(trade.closeTime.toString("dd.MM.yy hh:mm:ss")));
            if(trade.status == TradeStatus::OPEN) {
                ui->tableHistory->setItem(rowHistory, 7, new QTableWidgetItem("OPEN"));
            }
            else {
                ui->tableHistory->setItem(rowHistory, 7, new QTableWidgetItem("CLOSED"));
            }
            ui->chart_widget->createTakeProfitLine(trade.takeProfit, false);
            ui->chart_widget->createStopLossLine(trade.stopLoss, false);
            ui->tablePositions->removeRow(i);


        }
    }
}

void MainWindow::onProfitLossUpdated(int tradeId, double profitLoss)
{
    for(int i = 0; i < ui->tablePositions->rowCount(); ++i) {
        if(ui->tablePositions->item(i, 0)->text() == QString::number(tradeId)) {
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(profitLoss, 'f', 2));
            if(profitLoss >= 0) item->setForeground(QBrush(Qt::green));
            else item->setForeground(QBrush(Qt::red));
            ui->tablePositions->setItem(i, 5, item);
        }
    }

}

void MainWindow::onPositionDoubleClicked(int row, int column)
{
    int id = ui->tablePositions->item(row, 0)->text().toInt();
    TradeData trade = simulator->getTradeById(id);
    if(trade.status == TradeStatus::OPEN) {
        auto reply = QMessageBox::question(this, "Закрытие сделки", "Хотите закрыть сделку?", QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            QString ticker = ui->tablePositions->item(row, 1)->text();
            double currentPrice = 0.0;
            for(int i = 0; i < coinList.size(); ++i) {
                if(ticker == coinList[i]->getTicker()) {
                    currentPrice = coinList[i]->getPrice();
                }
            }
            simulator->closeTrade(id, currentPrice);
        }
    }
}

void MainWindow::updateUIFromSimulator()
{
    ui->label_Balance->setText("Баланс: " + QString::number(simulator->getBalance()) + "$");
    QList<TradeData> trades = simulator->getTrades();
    for(const TradeData& trade : trades) {
        if(trade.status == TradeStatus::OPEN) {
            int row = ui->tablePositions->rowCount();
            ui->tablePositions->insertRow(row);
            ui->tablePositions->setItem(row, 0, new QTableWidgetItem(QString::number(trade.id)));
            ui->tablePositions->setItem(row, 1, new QTableWidgetItem(trade.ticker));
            if(trade.type == TradeType::BUY) {
                ui->tablePositions->setItem(row, 2, new QTableWidgetItem("LONG"));

            }
            else {
                ui->tablePositions->setItem(row, 2, new QTableWidgetItem("SHORT"));
            }
            ui->tablePositions->setItem(row, 3, new QTableWidgetItem(QString::number(trade.entryPrice, 'f', 2)));
            ui->tablePositions->setItem(row, 4, new QTableWidgetItem(QString::number(trade.amount, 'f', 2)));
            ui->tablePositions->setItem(row, 5, new QTableWidgetItem(QString::number(trade.profitLoss, 'f', 2)));
            ui->tablePositions->setItem(row, 6, new QTableWidgetItem(trade.openTime.toString("dd.MM.yy hh:mm:ss")));
            if(trade.status == TradeStatus::OPEN) {
                ui->tablePositions->setItem(row, 7, new QTableWidgetItem("OPEN"));
            }
            else {
                ui->tablePositions->setItem(row, 7, new QTableWidgetItem("CLOSED"));
            }

        }
        else {
            int rowHistory = ui->tableHistory->rowCount();
            ui->tableHistory->insertRow(rowHistory);
            ui->tableHistory->setItem(rowHistory, 0, new QTableWidgetItem(QString::number(trade.id)));
            ui->tableHistory->setItem(rowHistory, 1, new QTableWidgetItem(trade.ticker));
            if(trade.type == TradeType::BUY) {
                ui->tableHistory->setItem(rowHistory, 2, new QTableWidgetItem("LONG"));

            }
            else {
                ui->tableHistory->setItem(rowHistory, 2, new QTableWidgetItem("SHORT"));
            }
            ui->tableHistory->setItem(rowHistory, 3, new QTableWidgetItem(QString::number(trade.entryPrice, 'f', 2)));
            ui->tableHistory->setItem(rowHistory, 4, new QTableWidgetItem(QString::number(trade.amount, 'f', 2)));
            QTableWidgetItem* item = new QTableWidgetItem(QString::number(trade.profitLoss, 'f', 2));
            if(trade.profitLoss >= 0) item->setForeground(QBrush(Qt::green));
            else item->setForeground(QBrush(Qt::red));
            ui->tableHistory->setItem(rowHistory, 5, item);
            ui->tableHistory->setItem(rowHistory, 6, new QTableWidgetItem(trade.closeTime.toString("dd.MM.yy hh:mm:ss")));

            if(trade.status == TradeStatus::OPEN) {
                ui->tableHistory->setItem(rowHistory, 7, new QTableWidgetItem("OPEN"));
            }
            else {
                ui->tableHistory->setItem(rowHistory, 7, new QTableWidgetItem("CLOSED"));
            }
        }

    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    simulator->saveToFile("trades.json");
}

MainWindow::~MainWindow()
{
    delete ui;
    qDeleteAll(coinList);
    coinList.clear();
}


void MainWindow::on_actionGraphSettings_triggered()
{
    SettingsDialog dialog(this);
    if(dialog.exec() == QDialog::Accepted) {
        ui->chart_widget->setShowMA(dialog.getShowMA());
        ui->chart_widget->setShowRSI(dialog.getShowRSI());
        ui->chart_widget->setShowMACD(dialog.getShowMACD());
        ui->chart_widget->setMAPeriod(dialog.getMAPeriod());
        ui->chart_widget->setRSIPeriod(dialog.getRSIPeriod());
        dialog.saveSettings();
    }
}


void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

