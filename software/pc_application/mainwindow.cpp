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

    labelCharts = new QLabel("Charts", this);
    buttonChartsClear = new QPushButton("Clear", this);
    buttonChartsStopResume = new QPushButton("Stop", this);

    layoutCharts->addWidget(labelCharts);
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
}

void MainWindow::onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg)
{
    qDebug() << measuredDist << ", " << realDist << ", " << accDeg << ", " << gyroDeg << ", " << angleDeg;
}














