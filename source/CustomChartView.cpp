#include "CustomChartView.h"
#include "qcandlestickset.h"
#include "qdatetime.h"
#include <QList>
#include <QAbstractAxis>
#include <QValueAxis>
#include <QBarCategoryAxis>
#include <QDateTimeAxis>
#include <QCandlestickSeries>
#include <QDateTime>


CustomChartView::CustomChartView(QChart* chart, QWidget* parrent = nullptr) : QChartView(chart, parrent) {
    isDragging = false;
    priceLabel = new QLabel(this);
    priceLabel->setStyleSheet("QLabel { color: black; font-size: 14px; }");
    priceLabel->hide();
}

void CustomChartView::setVerticalScrollEnabled(bool f)
{
    verticalScrollEnabled = f;
}

void CustomChartView::setAxisXFormat(QString f)
{
    format = f;
}


void CustomChartView::mousePressEvent(QMouseEvent *event)
{
    lastMouse = event->pos();
    isDragging = true;
}
void CustomChartView::mouseMoveEvent(QMouseEvent *event)
{
    QPointF chartPos = chart()->mapToValue(event->pos());

    // Покажи метку с ценой
    QDateTime time = QDateTime::fromMSecsSinceEpoch(chartPos.x());
    priceLabel->setText(QString("%1 \n%2").arg(chartPos.y(), 0, 'f', 2).arg(time.toString(format)));
    priceLabel->adjustSize();
    priceLabel->move(event->pos() + QPoint(15, -10));
    priceLabel->show();
    if (isDragging) {
        QPoint delta = event->pos() - lastMouse;

        QList<QAbstractAxis*> axesX = chart()->axes(Qt::Horizontal);
        QList<QAbstractAxis*> axesY = chart()->axes(Qt::Vertical);

        if (!axesX.isEmpty() && !axesY.isEmpty()) {
            // Ось Y (цена) - как раньше

            QValueAxis* axisY = qobject_cast<QValueAxis*>(axesY.first());
            if(verticalScrollEnabled) {
                if (axisY) {
                    qreal dy = axisY->max() - axisY->min();
                    qreal shiftY = dy * delta.y() / chart()->plotArea().height();
                    axisY->setRange(axisY->min() + shiftY, axisY->max() + shiftY);
                }
            }

            // Ось X (время) - новая логика
            QDateTimeAxis* axisX = qobject_cast<QDateTimeAxis*>(axesX.first());
            if (axisX) {
                // Получи границы данных из series
                QCandlestickSeries* series = qobject_cast<QCandlestickSeries*>(chart()->series().first());
                if (series && !series->sets().isEmpty()) {
                    qint64 dataMin = series->sets().first()->timestamp();
                    qint64 dataMax = series->sets().last()->timestamp();

                    // Вычисли новый диапазон
                    qint64 dx = axisX->max().toMSecsSinceEpoch() - axisX->min().toMSecsSinceEpoch();
                    qint64 shiftX = dx * delta.x() / chart()->plotArea().width();

                    qint64 newMinMs = axisX->min().toMSecsSinceEpoch() - shiftX;
                    qint64 newMaxMs = axisX->max().toMSecsSinceEpoch() - shiftX;

                    // Ограничь границами данных
                    // if (newMinMs < dataMin) {
                    //     newMinMs = dataMin;
                    //     newMaxMs = dataMin + dx;
                    // }
                    // if (newMaxMs > dataMax) {
                    //     newMaxMs = dataMax;
                    //     newMinMs = dataMax - dx;
                    // }

                    QDateTime newMin = QDateTime::fromMSecsSinceEpoch(newMinMs);
                    QDateTime newMax = QDateTime::fromMSecsSinceEpoch(newMaxMs);

                    axisX->setRange(newMin, newMax);

                    emit axisXRangeChanged(newMin, newMax);
                }

            }
        }

        lastMouse = event->pos();
    }

}

void CustomChartView::mouseReleaseEvent(QMouseEvent *event)
{
    isDragging = false;
}

