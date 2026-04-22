#ifndef CUSTOMCHARTVIEW_H
#define CUSTOMCHARTVIEW_H

#include <QChartView>
#include <QObject>
#include <QLabel>
#include <QDateTime>
class CustomChartView : public QChartView
{
    Q_OBJECT
public:
    CustomChartView(QChart*, QWidget*);
    void setVerticalScrollEnabled(bool);
    void setAxisXFormat(QString f);
    void showText(QString&);
protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
private:
    QPoint lastMouse;
    bool isDragging;
    QLabel* priceLabel;
    QLabel* text;
    bool verticalScrollEnabled = true;
    qreal factor = 1.0;
    QString format = "hh:mm";
signals:
    void axisXRangeChanged(QDateTime min, QDateTime max);
};

#endif // CUSTOMCHARTVIEW_H
