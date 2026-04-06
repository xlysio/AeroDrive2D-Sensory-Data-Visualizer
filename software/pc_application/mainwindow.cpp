#include "mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    resize(1600, 800);
    setMinimumSize(800, 400);
    setWindowTitle("AeroDrive2D");

    serialHandler = new SerialHandler(this);
    serialHandler->openPort("/dev/ttyACM0");

    QWidget *widgetWindow = new QWidget(this);
    setCentralWidget(widgetWindow);
    QHBoxLayout *layoutWindow = new QHBoxLayout(widgetWindow);

    QVBoxLayout *layoutGame = new QVBoxLayout();

    labelGame = new QLabel("Game", this);
    buttonGameStartRestart = new QPushButton("Start", this);

    layoutGame->addWidget(labelGame);
    layoutGame->addWidget(buttonGameStartRestart);

    buttonGameStartRestartState = false;

    layoutWindow->addLayout(layoutGame);

    QVBoxLayout *layoutCharts = new QVBoxLayout();
    QHBoxLayout *layoutChartsButtons = new QHBoxLayout();

    distanceChart = new ChartWidget("Distance", "d", 0.0, 500.0, this);
    distanceChart->addSeries("Measured", Qt::red);
    distanceChart->addSeries("Compensated", Qt::blue);

    angleChart = new ChartWidget("Angle", "ang", -90.0, 90.0, this);
    angleChart->addSeries("Accelerometer", Qt::red);
    angleChart->addSeries("Gyroscope", Qt::green);
    angleChart->addSeries("Complementary filter", Qt::blue);

    QHBoxLayout *layoutDistCheckboxes = new QHBoxLayout();
    layoutDistCheckboxes->addWidget(distanceChart->createSeriesCheckbox(0, this));
    layoutDistCheckboxes->addWidget(distanceChart->createSeriesCheckbox(1, this));

    QHBoxLayout *layoutAngleCheckboxes = new QHBoxLayout();
    layoutAngleCheckboxes->addWidget(angleChart->createSeriesCheckbox(0, this));
    layoutAngleCheckboxes->addWidget(angleChart->createSeriesCheckbox(1, this));
    layoutAngleCheckboxes->addWidget(angleChart->createSeriesCheckbox(2, this));

    buttonChartsClear = new QPushButton("Clear", this);
    buttonChartsStopResume = new QPushButton("Stop", this);

    layoutCharts->addWidget(distanceChart);
    layoutCharts->addLayout(layoutDistCheckboxes);
    layoutCharts->addWidget(angleChart);
    layoutCharts->addLayout(layoutAngleCheckboxes);
    layoutChartsButtons->addWidget(buttonChartsClear);
    layoutChartsButtons->addWidget(buttonChartsStopResume);
    layoutCharts->addLayout(layoutChartsButtons);

    buttonChartsStopResumeState = false;

    layoutWindow->addLayout(layoutCharts);

    layoutWindow->setStretch(0, 1);
    layoutWindow->setStretch(1, 1);

    connect(buttonGameStartRestart, &QPushButton::clicked, this, &MainWindow::onGameStartRestartClicked);
    connect(buttonChartsClear, &QPushButton::clicked, this, &MainWindow::onChartsClearClicked);
    connect(buttonChartsStopResume, &QPushButton::clicked, this, &MainWindow::onChartsStopResumeClicked);
    connect(serialHandler, &SerialHandler::dataReceived, this, &MainWindow::onSerialDataReceived);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onGameStartRestartClicked()
{
    if (buttonGameStartRestartState)
    {
        buttonGameStartRestart->setText("Start");
        buttonGameStartRestartState = false;
    }
    else
    {
        buttonGameStartRestart->setText("Restart");
        buttonGameStartRestartState = true;
    }

    qDebug() << "Start / Restart clicked";
}

void MainWindow::onChartsStopResumeClicked()
{
    if (buttonChartsStopResumeState)
    {
        buttonChartsStopResume->setText("Stop");
        buttonChartsStopResumeState = false;
    }
    else
    {
        buttonChartsStopResume->setText("Resume");
        buttonChartsStopResumeState = true;
    }

    qDebug() << "Stop / Resume clicked";
}

void MainWindow::onChartsClearClicked()
{
    qDebug() << "Clear clicked";

    distanceChart->clearChart();
    angleChart->clearChart();
}

void MainWindow::onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg)
{
    qDebug() << measuredDist << ", " << realDist << ", " << accDeg << ", " << gyroDeg << ", " << angleDeg;

    if (!buttonChartsStopResumeState)
    {
        distanceChart->addPoint(0, measuredDist);
        distanceChart->addPoint(1, realDist);
        angleChart->addPoint(0, accDeg);
        angleChart->addPoint(1, gyroDeg);
        angleChart->addPoint(2, angleDeg);
    }

}














