/**
 * @file gamewidget.cpp
 * @brief Implementation of the GameWidget class.
 */

#include "gamewidget.h"

GameWidget::GameWidget(QWidget *parent): QWidget{parent}
{
    buildTrack();

    // car initial state
    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    carSize = 0.04f;
    targetSpeed = 0.0f;
    onTrack = true;

    // physics coefficients
    accelerationFactor = 0.005f;
    maxSpeed = 0.01f;
    turnRate = 0.08f;
    grassSpeedFactor = 0.2f;
    borderMargin = 0.04f;

    // game state
    totalLaps = 3;
    currentLap = 0;
    gameRunning = false;
    countdownActive = false;
    countdownTime = 5;
    raceFinished = false;

    // controller input
    lastSteerAngle = 0.0f;
    lastSpeed = 0.0f;

    // checkpoint tracking
    checkpointsPassed = QList<bool>(checkpoints.size(), false);

    // game loop timer - 50 fps
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameWidget::gameTick);
}


void GameWidget::buildTrack()
{
    // track center path
    trackPath = QPainterPath();
    trackPath.moveTo(0.10, 0.70);
    trackPath.lineTo(0.10, 0.30);
    trackPath.quadTo(0.10, 0.10, 0.30, 0.10);
    trackPath.lineTo(0.40, 0.10);
    trackPath.quadTo(0.60, 0.10, 0.60, 0.30);
    trackPath.lineTo(0.60, 0.40);
    trackPath.quadTo(0.60, 0.50, 0.80, 0.55);
    trackPath.quadTo(0.90, 0.60, 0.90, 0.70);
    trackPath.lineTo(0.90, 0.80);
    trackPath.quadTo(0.875, 0.90, 0.80, 0.90);
    trackPath.lineTo(0.70, 0.90);
    trackPath.quadTo(0.65, 0.90, 0.55, 0.85);
    trackPath.quadTo(0.45, 0.80, 0.35, 0.80);
    trackPath.quadTo(0.2, 0.80, 0.10, 0.70);

    // start/finish line
    startLineBeginning = QPointF(0.06, 0.5);
    startLineEnd = QPointF(0.14, 0.5);

    // collision area
    QPainterPathStroker stroker;
    stroker.setWidth(0.08);
    stroker.setCapStyle(Qt::RoundCap);
    stroker.setJoinStyle(Qt::RoundJoin);
    trackArea = stroker.createStroke(trackPath);

    // checkpoints
    checkpoints.clear();
    checkpoints.append(QLineF(0.04, 0.3, 0.16, 0.3));
    checkpoints.append(QLineF(0.35, 0.04, 0.35, 0.16));
    checkpoints.append(QLineF(0.54, 0.35, 0.66, 0.35));
    checkpoints.append(QLineF(0.84, 0.75, 0.96, 0.75));
    checkpoints.append(QLineF(0.50, 0.76, 0.50, 0.90));

    // obstacle collision zones
    obstacles.clear();
    obstacles.append(QRectF(0.78, 0.02, 0.20, 0.28));
    obstacles.append(QRectF(0.03, 0.87, 0.28, 0.12));
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int w = width();
    int h = height();

    drawBackground(painter, w, h);
    drawTrack(painter, w, h);
    drawCar(painter, w, h);
    drawScenery(painter, w, h);
    drawCheckpoints(painter, w, h);
    drawHud(painter, w, h);
}

void GameWidget::drawBackground(QPainter &painter, int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);
    painter.fillRect(rect(), QColor(30, 140, 30));
}

void GameWidget::drawTrack(QPainter &painter, int w, int h)
{
    // scale normalized path to widget size
    QTransform transform;
    transform.scale(w, h);
    QPainterPath scaledTrack = transform.map(trackPath);

    // asphalt
    QPen trackPen(QColor(80, 80, 80));
    trackPen.setWidth(80);
    trackPen.setCapStyle(Qt::RoundCap);
    trackPen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(trackPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(scaledTrack);

    // start/finish line
    QPen startPen(Qt::white);
    startPen.setWidth(12);
    painter.setPen(startPen);
    painter.drawLine(QPointF(startLineBeginning.x() * w, startLineBeginning.y() * h),
                     QPointF(startLineEnd.x() * w, startLineEnd.y() * h));
}

void GameWidget::drawScenery(QPainter &painter, int w, int h)
{
    // forest
    painter.setPen(QPen(QColor(0, 80, 0), 2));
    painter.setBrush(QColor(20, 100, 20));

    QList<QPointF> trees = {{0.80, 0.05}, {0.87, 0.07}, {0.94, 0.10}, {0.96, 0.04},
                            {0.95, 0.17}, {0.89, 0.14}, {0.90, 0.21}, {0.96, 0.24},
                            {0.82, 0.12}, {0.84, 0.18}, {0.85, 0.25}, {0.91, 0.28}};

    float treeRadius = 0.03f * qMin(w, h);

    for (const QPointF &tree : trees)
    {
        painter.drawEllipse(QPointF(tree.x() * w, tree.y() * h), treeRadius, treeRadius);
    }

    // stands
    painter.setPen(QPen(QColor(65, 65, 65), 1));
    painter.setBrush(QColor(210, 180, 100));
    for (int row = 0; row < 5; ++row)
    {
        float y = 0.88 + row * 0.02;
        painter.drawRect(QRectF(0.04 * w, y * h, 0.26 * w, 0.02 * h));
    }

    // stands pillars
    painter.setPen(QPen(QColor(0, 0, 0), 0.5));
    painter.setBrush(QColor(130, 130, 130));
    painter.drawRect(QRectF(0.03 * w, 0.92 * h, 0.01 * w, 0.06 * h));
    painter.drawRect(QRectF(0.30 * w, 0.92 * h, 0.01 * w, 0.06 * h));
    painter.drawRect(QRectF(0.03 * w, 0.98 * h, 0.28 * w, 0.01 * h));
}

void GameWidget::drawCheckpoints(QPainter &painter, int w, int h)
{
    QPen checkpointPen(Qt::blue);
    checkpointPen.setWidth(4);
    painter.setPen(checkpointPen);

    for (int i = 0; i < checkpoints.size(); ++i)
    {
        QLineF cp = checkpoints[i];
        painter.drawLine(QPointF(cp.x1() * w, cp.y1() * h),
                         QPointF(cp.x2() * w, cp.y2() * h));
    }
}

void GameWidget::drawCar(QPainter &painter, int w, int h)
{
    painter.save();

    float cx = carPosition.x() * w;
    float cy = carPosition.y() * h;
    float size = carSize * qMin(w, h);

    painter.translate(cx, cy);
    painter.rotate(carAngle);

    // dimensions relative to size
    float bodyLength = size * 1.2f;
    float bodyWidth = size * 0.5f;
    float noseLength = size * 0.2f;
    float noseWidthFront = size * 0.2f;
    float wheelLength = size * 0.35f;
    float wheelWidth = size * 0.15f;
    float spoilerLength = size * 0.2f;
    float spoilerWidth = size * 0.7f;

    painter.setPen(Qt::NoPen);

    // body
    painter.setBrush(QColor(200, 20, 20));
    painter.drawRect(QRectF(-bodyLength * 0.5f, -bodyWidth * 0.5f, bodyLength, bodyWidth));

    // nose
    QPolygonF nose;
    nose << QPointF(bodyLength * 0.5f, -(bodyWidth + 2 * wheelWidth) * 0.5f)
         << QPointF(bodyLength * 0.5f + noseLength, -noseWidthFront * 0.5f)
         << QPointF(bodyLength * 0.5f + noseLength, noseWidthFront * 0.5f)
         << QPointF(bodyLength * 0.5f, (bodyWidth + 2 * wheelWidth) * 0.5f);
    painter.drawPolygon(nose);

    // wheels
    painter.setBrush(Qt::black);
    painter.drawRect(QRectF(-bodyLength * 0.4f, -bodyWidth * 0.5f - wheelWidth, wheelLength, wheelWidth));
    painter.drawRect(QRectF(-bodyLength * 0.4f, bodyWidth * 0.5f, wheelLength, wheelWidth));
    painter.drawRect(QRectF(bodyLength * 0.2f, -bodyWidth * 0.5f - wheelWidth, wheelLength, wheelWidth));
    painter.drawRect(QRectF(bodyLength * 0.2f, bodyWidth * 0.5f, wheelLength, wheelWidth));

    // spoiler
    painter.setBrush(QColor(30, 30, 30));
    painter.drawRect(QRectF(-bodyLength * 0.55f, -spoilerWidth * 0.5f, spoilerLength, spoilerWidth));

    painter.restore();
}

void GameWidget::drawHud(QPainter &painter, int w, int h)
{
    // countdown before race start
    if (countdownActive)
    {
        int secondsLeft = countdownTime - (int)(countdownTimer.elapsed() / 1000.0f);
        if (secondsLeft < 0)
        {
            secondsLeft = 0;
        }

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 60, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, QString::number(secondsLeft));
    }

    // lap counter and current time
    if (gameRunning)
    {
        float currentTime = lapTimer.elapsed() / 1000.0f;

        painter.setPen(Qt::white);
        painter.setFont(QFont("Arial", 16, QFont::Bold));
        painter.drawText(10, 30, tr("Lap: %1 / %2").arg(currentLap + 1).arg(totalLaps));
        painter.drawText(10, 55, tr("Time: %1 s").arg(currentTime, 0, 'f', 2));
    }

    // race results
    if (raceFinished)
    {
        painter.setFont(QFont("Arial", 16));
        QFontMetrics fm(painter.font());

        float totalTime = 0;
        for (int i = 0; i < lapTimes.size(); ++i)
        {
            totalTime += lapTimes[i];

            QString labelText = tr("Lap %1: ").arg(i + 1);
            QString timeText = tr("%1 s").arg(lapTimes[i], 0, 'f', 2);

            int x = w / 2 - 80;
            int y = h / 2 + 40 + i * 25;

            painter.setPen(Qt::white);
            painter.drawText(x, y, labelText);

            painter.setPen(Qt::yellow);
            painter.drawText(x + fm.horizontalAdvance(labelText), y, timeText);
        }

        painter.setFont(QFont("Arial", 20, QFont::Bold));
        QFontMetrics fmBold(painter.font());

        QString totalLabel = tr("Total: ");
        QString totalValue = tr("%1 s").arg(totalTime, 0, 'f', 2);

        int xTotal = w / 2 - 80;
        int yTotal = h / 2 + 40 + lapTimes.size() * 25 + 10;

        painter.setPen(Qt::white);
        painter.drawText(xTotal, yTotal, totalLabel);

        painter.setPen(Qt::yellow);
        painter.drawText(xTotal + fmBold.horizontalAdvance(totalLabel), yTotal, totalValue);
    }
}


void GameWidget::updateCar(float speed, float steerAngle)
{
    lastSpeed = speed;
    lastSteerAngle = steerAngle;
}

void GameWidget::startGame()
{
    // reset car
    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    targetSpeed = 0.0f;

    // reset race state
    currentLap = 0;
    lapTimes.clear();
    raceFinished = false;
    gameRunning = false;
    countdownActive = true;

    // reset checkpoints
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

    // reset game state
    raceFinished = false;
    gameRunning = false;
    countdownActive = false;

    // reset car
    carPosition = QPointF(0.1, 0.5);
    carAngle = -90.0f;
    carSpeed = 0.0f;
    targetSpeed = 0.0f;

    // reset race data
    currentLap = 0;
    lapTimes.clear();

    // reset checkpoints
    for (int i = 0; i < checkpointsPassed.size(); ++i)
    {
        checkpointsPassed[i] = false;
    }

    update();
}

void GameWidget::gameTick()
{
    // countdown phase
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

    // determine speed by mapping controller distance to target speed
    targetSpeed = (1.0f - (lastSpeed / 500.0f)) * maxSpeed;
    if (targetSpeed < 0)
    {
        targetSpeed = 0;
    }

    // reduce max speed on grass
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

    // smooth acceleration
    carSpeed += (targetSpeed - carSpeed) * accelerationFactor;

    // controller angle determines car orientation
    float steerAmount = lastSteerAngle * turnRate;
    carAngle += steerAmount;

    // move forward in current direction
    float angleRad = carAngle * M_PI / 180.0f;
    float dx = cos(angleRad) * carSpeed;
    float dy = sin(angleRad) * carSpeed;

    QPointF previousPosition = carPosition;
    carPosition.setX(carPosition.x() + dx);
    carPosition.setY(carPosition.y() + dy);

    // window boundaries
    carPosition.setX(qBound((double)borderMargin, carPosition.x(), 1.0 - (double)borderMargin));
    carPosition.setY(qBound((double)borderMargin, carPosition.y(), 1.0 - (double)borderMargin));

    // obstacle collision
    for (const QRectF &obstacle: obstacles)
    {
        if (obstacle.contains(carPosition))
        {
            carPosition = previousPosition;
            carSpeed = 0.0f;
            break;
        }
    }

    // track detection
    onTrack = trackArea.contains(carPosition);

    // checkpoint detection
    QLineF moveLine(previousPosition, carPosition);
    QPointF intersection;

    for (int i = 0; i < checkpoints.size(); ++i)
    {
        if (!checkpointsPassed[i] && (moveLine.intersects(checkpoints[i], &intersection) == QLineF::BoundedIntersection))
        {
            checkpointsPassed[i] = true;
        }
    }

    QLineF startLine(startLineBeginning, startLineEnd);
    if (moveLine.intersects(startLine, &intersection) == QLineF::BoundedIntersection)
    {
        bool allCheckpointsPassed;
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
