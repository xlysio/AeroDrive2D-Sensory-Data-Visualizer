#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QPointF>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    void buildTrack();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPainterPath trackPath;

    QPointF startLineBeginning;
    QPointF startLineEnd;

signals:
};

#endif // GAMEWIDGET_H
