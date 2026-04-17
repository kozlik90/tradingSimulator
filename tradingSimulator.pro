QT       += core gui
QT += network
QT += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += includes/

SOURCES += \
    source/BybitApi.cpp \
    source/ChartWidget.cpp \
    source/CoinData.cpp \
    source/CustomChartView.cpp \
    source/CustomScrollArea.cpp \
    source/SettingsDialog.cpp \
    source/TechnicalIndicators.cpp \
    source/TradingSignals.cpp \
    source/TradingSimulator.cpp \
    source/main.cpp \
    source/mainwindow.cpp

HEADERS += \
    includes/BybitApi.h \
    includes/CandleData.h \
    includes/ChartWidget.h \
    includes/ChartWidget_копия.h \
    includes/CoinData.h \
    includes/CustomChartView.h \
    includes/CustomScrollArea.h \
    includes/SettingsDialog.h \
    includes/TechnicalIndicators.h \
    includes/TickerData.h \
    includes/TradeData.h \
    includes/TradingSignals.h \
    includes/TradingSimulator.h \
    includes/mainwindow.h

FORMS += \
    SettingsDialog.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
