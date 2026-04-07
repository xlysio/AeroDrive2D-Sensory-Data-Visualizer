/**
 * @file mainwindow.h
 * @brief Declaration of the MainWindow class.
 *
 * MainWindow is the central widget of the application.
 * It manages the game area, the charts area and serial communication with the physical controller.
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "serialhandler.h"
#include "chartwidget.h"

/**
 * @class MainWindow
 * @brief Main application window that controlls the game, charts and controls.
 *
 * The window is divided into two parts.
 * The left side contains a 2D racing game area and a Start/Restart button.
 * The right side contains distance and angle data charts, a Stop/Resume button and checkboxes that control visibility of the graphs.
 *
 * MainWindow receives sensor data from SerialHandler and distributes to the chart widgets for visualization.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the main window and initializes all child widgets.
     * @param parent Optional parent widget.
     */
    explicit MainWindow(QWidget *parent = nullptr);

    /**
     * @brief MainWindows's destructors.
     */
    ~MainWindow() override;

private:
    /**
     * @brief Placeholder label for the game area.
     */
    QLabel *labelGame;

    /**
     * @brief Button to start or restart the game.
     */
    QPushButton *buttonGameStartRestart;

    /**
     * @brief Flag to track the current state of the Start/Restart button.
     */
    bool buttonGameStartRestartState;

    /**
     * @brief Chart widget that displays distance sensor data.
     */
    ChartWidget *distanceChart;

    /**
     * @brief Chart widget that displays angle sensor data.
     */
    ChartWidget *angleChart;

    /**
     * @brief Button to clear all chart data.
     */
    QPushButton *buttonChartsClear;

    /**
     * @brief Button to stop or resume chart updates.
     */
    QPushButton *buttonChartsStopResume;

    /**
     * @brief Flag to track the current state of the Stop/Resume button.
     */
    bool buttonChartsStopResumeState;

    /**
     * @brief Handler for serial port communication with the controller.
     */
    SerialHandler *serialHandler;

private slots:
    /**
     * @brief Slot that triggers when the Start/Restart button is clicked.
     *
     * Toggles the button text between "Start" and "Restart" and updates the state flag (for now).
     */
    void onGameStartRestartClicked();

    /**
     * @brief Slot that triggers when the Stop/Resume charts button is cliked.
     *
     * Toggles the button text between "Stop" and "Resume" and updates the state flag.
     * When stopped, new sensor data is not added to the charts.
     */
    void onChartsStopResumeClicked();

    /**
     * @brief Slot that triggers when the Clear charts button is cliked.
     *
     * Clears all data series in both the distance and angle charts and resets the time axis.
     */
    void onChartsClearClicked();

    /**
     * @brief Slot that receives parsed sensor data from SerialHandler.
     *
     * Distributes the received values to the charts unless chart updates are paused.
     *
     * @param measuredDist Distance measurement from distance sensor [mm].
     * @param realDist Compensated distance [mm].
     * @param accDeg Roll angle computed from accelerometer [degrees].
     * @param gyroDeg Roll angle integrated from gyroscope [degrees].
     * @param angleDeg Roll angle after complementary filter [degrees].
     */
    void onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg);
};

#endif // MAINWINDOW_H