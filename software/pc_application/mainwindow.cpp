/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class.
 */

#include "mainwindow.h"
#include <QDebug>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent)
{
    // window properties
    resize(1600, 800);
    setMinimumSize(800, 400);
    setWindowTitle(tr("AeroDrive2D"));

    // translator
    translator = new QTranslator(this);

    // serial communication
    serialHandler = new SerialHandler(this);
    buttonSerialConnectDisconnectState = false;

    // central widget and main layout
    QWidget *widgetWindow = new QWidget(this);
    setCentralWidget(widgetWindow);
    QHBoxLayout *layoutWindow = new QHBoxLayout(widgetWindow);

    // left side - game
    QVBoxLayout *layoutGame = new QVBoxLayout();

    gameWidget = new GameWidget(this);
    buttonGameStartRestart = new QPushButton(tr("Start"), this);

    layoutGame->addWidget(gameWidget);
    layoutGame->addWidget(buttonGameStartRestart);

    buttonGameStartRestartState = false;

    layoutWindow->addLayout(layoutGame);

    QVBoxLayout *layoutCharts = new QVBoxLayout();
    QHBoxLayout *layoutChartsButtons = new QHBoxLayout();
    QHBoxLayout *layoutFlags = new QHBoxLayout();

    buttonLanguageEnglish = new QToolButton(this);
    buttonLanguageEnglish->setText("ENG");
    buttonLanguageEnglish->setFixedSize(40,30);

    buttonLanguagePolish = new QToolButton(this);
    buttonLanguagePolish->setText("PL");
    buttonLanguagePolish->setFixedSize(40,30);

    layoutFlags->addStretch();
    layoutFlags->addWidget(buttonLanguageEnglish);
    layoutFlags->addWidget(buttonLanguagePolish);

    distanceChart = new ChartWidget(tr("Distance"), tr("d [mm]"), 0.0, 500.0, this);
    distanceChart->addSeries(tr("Measured"), Qt::red);
    distanceChart->addSeries(tr("Compensated"), Qt::blue);

    angleChart = new ChartWidget(tr("Angle"), tr("ang [°]"), -90.0, 90.0, this);
    angleChart->addSeries(tr("Accelerometer"), Qt::red);
    angleChart->addSeries(tr("Gyroscope"), Qt::green);
    angleChart->addSeries(tr("Complementary filter"), Qt::blue);

    QHBoxLayout *layoutDistCheckboxes = new QHBoxLayout();
    distanceCheckboxes.append(distanceChart->createSeriesCheckbox(0, this));
    distanceCheckboxes.append(distanceChart->createSeriesCheckbox(1, this));
    layoutDistCheckboxes->addWidget(distanceCheckboxes[0]);
    layoutDistCheckboxes->addWidget(distanceCheckboxes[1]);

    QHBoxLayout *layoutAngleCheckboxes = new QHBoxLayout();
    angleCheckboxes.append(angleChart->createSeriesCheckbox(0, this));
    angleCheckboxes.append(angleChart->createSeriesCheckbox(1, this));
    angleCheckboxes.append(angleChart->createSeriesCheckbox(2, this));
    layoutAngleCheckboxes->addWidget(angleCheckboxes[0]);
    layoutAngleCheckboxes->addWidget(angleCheckboxes[1]);
    layoutAngleCheckboxes->addWidget(angleCheckboxes[2]);

    buttonChartsClear = new QPushButton(tr("Clear"), this);
    buttonChartsStopResume = new QPushButton(tr("Stop"), this);
    buttonSerialConnectDisconnect = new QPushButton(tr("Connect"), this);

    layoutCharts->addLayout(layoutFlags);
    layoutCharts->addWidget(distanceChart);
    layoutCharts->addLayout(layoutDistCheckboxes);
    layoutCharts->addWidget(angleChart);
    layoutCharts->addLayout(layoutAngleCheckboxes);
    layoutChartsButtons->addWidget(buttonChartsClear);
    layoutChartsButtons->addWidget(buttonChartsStopResume);
    layoutChartsButtons->addWidget(buttonSerialConnectDisconnect);
    layoutCharts->addLayout(layoutChartsButtons);

    buttonChartsStopResumeState = false;

    layoutWindow->addLayout(layoutCharts);

    layoutWindow->setStretch(0, 1);
    layoutWindow->setStretch(1, 1);

    connect(buttonGameStartRestart, &QPushButton::clicked, this, &MainWindow::onGameStartRestartClicked);
    connect(buttonChartsClear, &QPushButton::clicked, this, &MainWindow::onChartsClearClicked);
    connect(buttonChartsStopResume, &QPushButton::clicked, this, &MainWindow::onChartsStopResumeClicked);
    connect(buttonSerialConnectDisconnect, &QPushButton::clicked, this, &MainWindow::onSerialConnectDisconnectClicked);
    connect(serialHandler, &SerialHandler::dataReceived, this, &MainWindow::onSerialDataReceived);
    connect(buttonLanguageEnglish, &QToolButton::clicked, this, &MainWindow::switchToEnglish);
    connect(buttonLanguagePolish, &QToolButton::clicked, this, &MainWindow::switchToPolish);
}

MainWindow::~MainWindow()
{
}

void MainWindow::onGameStartRestartClicked()
{
    if (buttonGameStartRestartState)
    {
        gameWidget->resetGame();
        buttonGameStartRestart->setText(tr("Start"));
        buttonGameStartRestartState = false;
    }
    else
    {
        gameWidget->startGame();
        buttonGameStartRestart->setText(tr("Restart"));
        buttonGameStartRestartState = true;
    }

    // qDebug() << "Start / Restart clicked";
}

void MainWindow::onChartsStopResumeClicked()
{
    if (buttonChartsStopResumeState)
    {
        buttonChartsStopResume->setText(tr("Stop"));
        buttonChartsStopResumeState = false;
    }
    else
    {
        buttonChartsStopResume->setText(tr("Resume"));
        buttonChartsStopResumeState = true;
    }

    // qDebug() << "Stop / Resume clicked";
}

void MainWindow::onChartsClearClicked()
{
    // qDebug() << "Clear clicked";

    distanceChart->clearChart();
    angleChart->clearChart();
}

void MainWindow::onSerialConnectDisconnectClicked()
{
    if (buttonSerialConnectDisconnectState)
    {
        serialHandler->closePort();
        buttonSerialConnectDisconnect->setText(tr("Connect"));
        buttonSerialConnectDisconnectState = false;
    }
    else
    {
        if (serialHandler->openPort("/dev/ttyACM0"))
        {
            buttonSerialConnectDisconnect->setText(tr("Disconnect"));
            buttonSerialConnectDisconnectState = true;
        }
    }
}

void MainWindow::onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg)
{
    // qDebug() << measuredDist << ", " << realDist << ", " << accDeg << ", " << gyroDeg << ", " << angleDeg;

    if (!buttonChartsStopResumeState)
    {
        distanceChart->addPoint(0, measuredDist);
        distanceChart->addPoint(1, realDist);
        angleChart->addPoint(0, accDeg);
        angleChart->addPoint(1, gyroDeg);
        angleChart->addPoint(2, angleDeg);
    }

    gameWidget->updateCar(realDist, angleDeg);
}

void MainWindow::switchToEnglish()
{
    qApp->removeTranslator(translator);
    retranslateUi();
}

void MainWindow::switchToPolish()
{
    if (translator->load("app_pl", QCoreApplication::applicationDirPath()))
    {
        qApp->installTranslator(translator);
        // qDebug() << "Loaded";
    }
    else
    {
        // qDebug() << "Not loaded";
    }
    retranslateUi();
}

void MainWindow::retranslateUi()
{
    setWindowTitle(tr("AeroDrive2D"));

    if (buttonGameStartRestartState)
    {
        buttonGameStartRestart->setText(tr("Restart"));
    }
    else
    {
        buttonGameStartRestart->setText(tr("Start"));
    }

    buttonChartsClear->setText(tr("Clear"));

    if (buttonChartsStopResumeState)
    {
        buttonChartsStopResume->setText(tr("Resume"));
    }
    else
    {
        buttonChartsStopResume->setText(tr("Stop"));
    }

    if (buttonSerialConnectDisconnectState)
    {
        buttonSerialConnectDisconnect->setText(tr("Disconnect"));
    }
    else
    {
        buttonSerialConnectDisconnect->setText(tr("Connect"));
    }

    distanceChart->retranslate(tr("Distance"), tr("d [mm]"), {tr("Measured"), tr("Compensated")});
    QStringList distanceCheckboxesNames = {tr("Measured"), tr("Compensated")};
    for (int i = 0; i < distanceCheckboxes.size(); ++i)
    {
        distanceCheckboxes[i]->setText(distanceCheckboxesNames[i]);
    }

    angleChart->retranslate(tr("Angle"), tr("ang [°]"), {tr("Accelerometer"), tr("Gyroscope"), tr("Complementary filter")});
    QStringList angleCheckboxesNames = {tr("Accelerometer"), tr("Gyroscope"),  tr("Complementary filter")};
    for (int i = 0; i < angleCheckboxes.size(); ++i)
    {
        angleCheckboxes[i]->setText(angleCheckboxesNames[i]);
    }
}