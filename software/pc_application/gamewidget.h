/**
 * @file gamewidget.h
 * @brief Declaration of the GameWidget class.
 *
 * GameWidget implemens a 2D racing game controlled by physical sensor data.
 * It renders the track, car, scenery and handles game logic.
 */

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

/**
 * @class GameWidget
 * @brief A 2D racing game widget controlled by sensor input.
 *
 * The game features a track with checkpoints, obstacle collision, grass slowdown, countdown timer and lap time measurement.
 * Car speed is determined by controller's distance from the desk and steering is determined by controller's roll angle.
 */
class GameWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Constructs the game widget, builds the track, and initializes all state flags.
     * @param parent - Optional parent widget.
     */
    explicit GameWidget(QWidget *parent = nullptr);

    /**
     * @brief Defines track geometry, checkpoints, and obstacles.
     */
    void buildTrack();

    /**
     * @brief Receives controller input to be applied in the next game tick.
     * @param speed - Compensated distance from ToF sensor [mm].
     * @param steerAngle - Roll angle after complementary filter [degrees].
     */
    void updateCar(float speed, float steerAngle);

    /**
     * @brief Starts the game with a countdown timer.
     */
    void startGame();

    /**
     * @brief Stops the game and resets all state flags to initial values.
     */
    void resetGame();

protected:
    /**
     * @brief Renders the entire game scene.
     */
    void paintEvent(QPaintEvent *event) override;

private:
    /**
     * @brief Draws the grass background.
     */
    void drawBackground(QPainter &painter, int w, int h);

    /**
     * @brief Draws the asphalt track and the start/finish line.
     */
    void drawTrack(QPainter &painter, int w, int h);

    /**
     * @brief Draws scenery elements (trees, stands).
     */
    void drawScenery(QPainter &painter, int w, int h);

    /**
     * @brief Draws checkpoint lines on the track.
     */
    void drawCheckpoints(QPainter &painter, int w, int h);

    /**
     * @brief Draws the racing car (body, nose, wheels, spoiler).
     */
    void drawCar(QPainter &painter, int w, int h);

    /**
     * @brief Draws the hud (countdown, lap info, race results).
     */
    void drawHud(QPainter &painter, int w, int h);

    /**
     * @brief Center path of the track in normalized coordinates.
     */
    QPainterPath trackPath;

    /**
     * @brief Stroked version of trackPath used for on-track collision detection.
     */
    QPainterPath trackArea;

    /**
     * @brief Start/finish line endpoints in normalized coordinates.
     */
    QPointF startLineBeginning;
    QPointF startLineEnd;

    /**
     * @brief Checkpoint lines crossing the track.
     */
    QList<QLineF> checkpoints;

    /**
     * @brief Tracks which checkpoints have been passed in the current lap.
     */
    QList<bool> checkpointsPassed;

    /**
     * @brief Rectangular obstacle collision zones.
     */
    QList<QRectF> obstacles;

    /**
     * @brief Current car position in normalized coordinates.
     */
    QPointF carPosition;

    /**
     * @brief Current car orientation [degrees].
     */
    float carAngle;

    /**
     * @brief Current car speed in normalized units.
     */
    float carSpeed;

    /**
     * @brief Target speed determined from controller distance.
     */
    float targetSpeed;

    /**
     * @brief Size of the car relative to widget dimensions.
     */
    float carSize;

    /**
     * @brief Flag to track whether the car is currently on the track.
     */
    bool onTrack;

    /**
     * @brief Smoothing factor for speed transitions (0.0–1.0).
     */
    float accelerationFactor;

    /**
     * @brief Maximum car speed in normalized units.
     */
    float maxSpeed;

    /**
     * @brief Multiplier converting controller angle to car rotation.
     */
    float turnRate;

    /**
     * @brief Speed multiplier when driving on grass.
     */
    float grassSpeedFactor;

    /**
     * @brief Minimum distance from window edge in normalized coordinates.
     */
    float borderMargin;

    /**
     * @brief Last received steering angle from the controller [degrees].
     */
    float lastSteerAngle;

    /**
     * @brief Last received distance value from the controller [mm].
     */
    float lastSpeed;

    /**
     * @brief Timer that runs the game loop at 50 fps.
     */
    QTimer *gameTimer;

    /**
     * @brief Measures elapsed time for the current lap.
     */
    QElapsedTimer lapTimer;

    /**
     * @brief Measures elapsed time during countdown at the beginning.
     */
    QElapsedTimer countdownTimer;

    /**
     * @brief Duration of the countdown in seconds.
     */
    int countdownTime;

    /**
     * @brief Current lap number.
     */
    int currentLap;

    /**
     * @brief Total number of laps.
     */
    int totalLaps;

    /**
     * @brief Holds time for each completed lap.
     */
    QList<float> lapTimes;

    /**
     * @brief Flag to track whether the race is running.
     */
    bool gameRunning;

    /**
     * @brief Flag to track whether the countdown is active.
     */
    bool countdownActive;

    /**
     * @brief Flag that tracks whether all laps have been completed.
     */
    bool raceFinished;

private slots:
    /**
     * @brief Main game loop called every 20 ms.
     *
     * Handles countdown, car physics, collision detection, checkpoint tracking, and lap completion.
     */
    void gameTick();
};

#endif // GAMEWIDGET_H
