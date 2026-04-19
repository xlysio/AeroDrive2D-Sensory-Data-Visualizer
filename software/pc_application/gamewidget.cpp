#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget{parent}
{
    this->buildTrack();
}

void GameWidget::buildTrack()
{
    trackPath = QPainterPath();
    trackPath.moveTo(0.1, 0.7);
    trackPath.lineTo(0.1, 0.3);
    trackPath.quadTo(0.1, 0.1, 0.3, 0.1);
    trackPath.lineTo(0.4, 0.1);
    trackPath.quadTo(0.6, 0.1, 0.6, 0.3);
    trackPath.lineTo(0.6, 0.4);
    trackPath.quadTo(0.6, 0.5, 0.8, 0.55);
    trackPath.quadTo(0.9, 0.6, 0.9, 0.7);
    trackPath.lineTo(0.9, 0.80);
    trackPath.quadTo(0.875, 0.9, 0.8, 0.9);
    trackPath.lineTo(0.7, 0.9);
    trackPath.quadTo(0.65, 0.9, 0.55, 0.85);
    trackPath.quadTo(0.45, 0.8, 0.35, 0.8);
    trackPath.quadTo(0.2, 0.8, 0.1, 0.7);

    startLineBeginning = QPointF(0.06, 0.5);
    startLineEnd = QPointF(0.14, 0.5);
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    // background;
    painter.fillRect(rect(), QColor(30, 140, 30));

    // scaling
    QTransform transform;
    transform.scale(w, h);
    QPainterPath scaledTrack = transform.map(trackPath);

    // drawing the circuit
    QPen trackPen(QColor(80, 80, 80));
    trackPen.setWidth(80);
    trackPen.setCapStyle(Qt::RoundCap);
    trackPen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(trackPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(scaledTrack);

    // start / finish line
    QPen startPen(Qt::white);
    startPen.setWidth(12);
    painter.setPen(startPen);
    painter.drawLine(QPointF(startLineBeginning.x() * w, startLineBeginning.y() * h),
                     QPointF(startLineEnd.x() * w, startLineEnd.y() * h));

}