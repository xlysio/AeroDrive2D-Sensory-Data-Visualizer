#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent)
    : QWidget{parent}
{
    this->buildTrack();

    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    carSize = 0.05f;

    accelerationFactor = 0.01;
    maxSpeed = 0.01f;
    turnRate = 0.05f;
    grassSpeedFactor = 0.2f;
    borderMargin = 0.02f;
    totalLaps = 2;

    targetSpeed = 0.0f;
    onTrack = true;
    currentLap = 0;
    gameRunning = false;
    countdownActive = false;
    countdownTime = 5;
    raceFinished = false;
    lastSteerAngle = 0.0f;
    lastSpeed = 0.0f;
    nextCheckpoint = 0;

    checkpointsPassed.clear();
    checkpointsPassed = QList<bool>(checkpoints.size(), false);

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameWidget::gameTick);
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

    QPainterPathStroker stroker;
    stroker.setWidth(0.08);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    trackArea = stroker.createStroke(trackPath);

    checkpoints.clear();
    checkpoints.append(QLineF(0.04, 0.3, 0.16, 0.3));
    checkpoints.append(QLineF(0.35, 0.04, 0.35, 0.16));
    checkpoints.append(QLineF(0.54, 0.35, 0.66, 0.35));
    checkpoints.append(QLineF(0.84, 0.75, 0.96, 0.75));
    checkpoints.append(QLineF(0.50, 0.76, 0.50, 0.90));
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    painter.fillRect(rect(), QColor(30, 140, 30));

    QTransform transform;
    transform.scale(w, h);
    QPainterPath scaledTrack = transform.map(trackPath);

    QPen trackPen(QColor(80, 80, 80));
    trackPen.setWidth(60);
    trackPen.setCapStyle(Qt::RoundCap);
    trackPen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(trackPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(scaledTrack);

    QPen startPen(Qt::white);
    startPen.setWidth(12);
    painter.setPen(startPen);
    painter.drawLine(QPointF(startLineBeginning.x() * w, startLineBeginning.y() * h),
                     QPointF(startLineEnd.x() * w, startLineEnd.y() * h));


    painter.save();

    float cx = carPosition.x() * w;
    float cy = carPosition.y() * h;
    float size = carSize * qMin(w, h);

    painter.translate(cx, cy);
    painter.rotate(carAngle);

    QPolygonF carShape;
    carShape << QPointF(size, 0) << QPointF(-size * 0.6, -size * 0.5) << QPointF(-size * 0.6, size * 0.5);

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::red);
    painter.drawPolygon(carShape);

    painter.restore();

    QPen checkpointPen(Qt::blue);
    checkpointPen.setWidth(4);
    painter.setPen(checkpointPen);
    for (int i = 0; i < checkpoints.size(); ++i)
    {
        QLineF checkpoint = checkpoints[i];
        painter.drawLine(QPointF(checkpoint.x1() * w, checkpoint.y1() * h),
                         QPointF(checkpoint.x2() * w, checkpoint.y2() * h));
    }

    if (countdownActive)
    {
        int secondsLeft = countdownTime - (int)(countdownTimer.elapsed() / 1000.0f);
        if(secondsLeft < 0)
        {
            secondsLeft = 0;
        }

        painter.setPen(Qt::white);
        QFont countdownFont("Arial", 60, QFont::Bold);
        painter.setFont(countdownFont);
        painter.drawText(rect(), Qt::AlignCenter, QString::number(secondsLeft));
    }

    if (gameRunning)
    {
        float currentTime = lapTimer.elapsed() / 1000.f;

        painter.setPen(Qt::white);
        QFont infoFont("Arial", 16, QFont::Bold);
        painter.setFont(infoFont);
        painter.drawText(10, 30, QString("Lap: %1 / %2").arg(currentLap + 1).arg(totalLaps));
        painter.drawText(10, 55, QString("Time: %1 s").arg(currentTime, 0, 'f', 2));
    }

    if (raceFinished)
    {
        painter.setPen(Qt::white);
        QFont lapFont("Arial", 16);
        painter.setFont(lapFont);

        float totalTime = 0;
        for (int i = 0; i < lapTimes.size(); ++i)
        {
            totalTime += lapTimes[i];
            QString lapText = QString("Lap %1: %2 s").arg(i + 1).arg(lapTimes[i], 0, 'f', 2);
            painter.drawText(w / 2 - 80, h / 2 + 40 + i * 25, lapText);
        }

        QString totalText = QString("Total: %1 s").arg(totalTime, 0, 'f', 2);
        QFont totalFont("Arial", 20, QFont::Bold);
        painter.setFont(totalFont);
        painter.drawText(w / 2 - 80, h / 2 + 40 + lapTimes.size() * 25 + 10, totalText);
    }
}

void GameWidget::updateCar(float speed, float steerAngle)
{
    lastSpeed = speed;
    lastSteerAngle = steerAngle;
}

void GameWidget::startGame()
{
    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    targetSpeed = 0.0f;
    currentLap = 0;
    lapTimes.clear();
    raceFinished = false;
    gameRunning = false;
    countdownActive = true;
    nextCheckpoint = 0;

    for (int i = 0; i < checkpointsPassed.size(); ++i)
    {
        checkpointsPassed[i] = false;
    }

    countdownTimer.start();
    gameTimer->start(20);
}

void GameWidget::resetGame()
{
    gameTimer->stop();
    raceFinished = false;
    gameRunning = false;
    countdownActive = false;
    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    targetSpeed = 0.0f;
    currentLap = 0;
    lapTimes.clear();

    for (int i = 0; i < checkpointsPassed.size(); ++i)
    {
        checkpointsPassed[i] = false;
    }

    update();
}

void GameWidget::gameTick()
{
    if (countdownActive)
    {
        if (countdownTimer.elapsed() >= countdownTime * 1000)
        {
            countdownActive = false;
            gameRunning = true;
            lapTimer.start();
        }
        update();
        return;
    }

    if (!gameRunning || raceFinished)
    {
        update();
        return;
    }

    targetSpeed = (1.0f - (lastSpeed / 500.0f)) * maxSpeed;
    if (targetSpeed < 0)
    {
        targetSpeed = 0;
    }

    float actualMaxSpeed;
    if (onTrack)
    {
        actualMaxSpeed = maxSpeed;
    }
    else
    {
        actualMaxSpeed = maxSpeed * grassSpeedFactor;
    }

    if (targetSpeed > actualMaxSpeed)
    {
        targetSpeed = actualMaxSpeed;
    }

    carSpeed += (targetSpeed - carSpeed) * accelerationFactor;

    float steerAmount = lastSteerAngle * turnRate;
    carAngle += steerAmount;

    float angleRad = carAngle * M_PI / 180.0f;
    float dx = cos(angleRad) * carSpeed;
    float dy = sin(angleRad) * carSpeed;

    QPointF previousPosition = carPosition;
    carPosition.setX(carPosition.x() + dx);
    carPosition.setY(carPosition.y() + dy);

    carPosition.setX(qBound((double)borderMargin, carPosition.x(), 1.0 - (double)borderMargin));
    carPosition.setY(qBound((double)borderMargin, carPosition.y(), 1.0 - (double)borderMargin));

    onTrack = trackArea.contains(carPosition);

    QLineF moveLine(previousPosition, carPosition);
    QPointF checkpointIntersection;
    for (int i = 0; i < checkpoints.size(); ++i)
    {
        if (!checkpointsPassed[i] && (moveLine.intersects(checkpoints[i], &checkpointIntersection) == QLineF::BoundedIntersection))
        {
            checkpointsPassed[i] = true;
        }
    }

    QLineF startLine(startLineBeginning, startLineEnd);
    QPointF startIntersection;
    bool allCheckpointsPassed = true;
    if (allCheckpointsPassed && (moveLine.intersects(startLine, &startIntersection) == QLineF::BoundedIntersection))
    {
        for (int i = 0; i < checkpointsPassed.size(); ++i)
        {
            if (!checkpointsPassed[i])
            {
                allCheckpointsPassed = false;
                break;
            }
        }

        if (allCheckpointsPassed)
        {
            float lapTime = lapTimer.elapsed() / 1000.0f;
            lapTimes.append(lapTime);
            ++currentLap;

            if (currentLap >= totalLaps)
            {
                raceFinished = true;
                gameRunning = false;
            }
            else
            {
                lapTimer.restart();
            }

            for (int i = 0; i < checkpointsPassed.size(); ++i)
            {
                checkpointsPassed[i] = false;
            }
        }
    }

    update();
}
