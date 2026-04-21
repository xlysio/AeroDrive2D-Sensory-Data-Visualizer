#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPolygonF>
#include <QPointF>
#include <QTimer>
#include <QLineF>
#include <QPainterPathStroker>
#include <QElapsedTimer>
#include <cmath>

class GameWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameWidget(QWidget *parent = nullptr);
    void buildTrack();
    void updateCar(float speed, float steerAngle);
    void startGame();
    void resetGame();
    void setRaceFinished(bool newState);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPainterPath trackPath;
    QPainterPath trackArea;

    QList<QLineF> checkpoints;
    QList<bool> checkpointsPassed;
    int nextCheckpoint;

    QPointF startLineBeginning;
    QPointF startLineEnd;

    QPointF carPosition;
    float carAngle;
    float carSpeed;
    float carSize;
    float targetSpeed;
    bool onTrack;

    float accelerationFactor;
    float maxSpeed;
    float turnRate;
    float grassSpeedFactor;
    float borderMargin;

    QTimer *gameTimer;
    QElapsedTimer lapTimer;
    QElapsedTimer countdownTimer;
    int countdownTime;
    int currentLap;
    int totalLaps;
    QList<float> lapTimes;
    bool gameRunning;
    bool countdownActive;
    bool raceFinished;
    float lastSteerAngle;
    float lastSpeed;
    QList<QRectF> obstacles;

private slots:
    void gameTick();
};

#endif // GAMEWIDGET_H
