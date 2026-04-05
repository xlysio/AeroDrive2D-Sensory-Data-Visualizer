#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>
#include "serialhandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private:
    QLabel *labelGame;
    QPushButton *buttonGameStartRestart;
    bool buttonGameStartRestartState;

    QLabel *labelCharts;
    QPushButton *buttonChartsClear;
    QPushButton *buttonChartsStopResume;
    bool buttonChartsStopResumeState;
    \
    SerialHandler *serialHandler;

private slots:
    void onGameStartRestartClicked();
    void onChartsStopResumeClicked();
    void onChartsClearClicked();
    void onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg);
};

#endif // MAINWINDOW_H