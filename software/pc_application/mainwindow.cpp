/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class.
 */

#include "mainwindow.h"
#include <QDebug>
#include <QCoreApplication>
#include <QIcon>

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
    QHBoxLayout *layoutGameButton = new QHBoxLayout();

    gameWidget = new GameWidget(this);

    buttonGameStartRestart = new QPushButton(tr("Start"), this);
    buttonGameStartRestartState = false;

    layoutGame->addWidget(gameWidget);
    layoutGameButton->addStretch(1);
    layoutGameButton->addWidget(buttonGameStartRestart, 1);
    layoutGameButton->addStretch(1);
    layoutGame->addLayout(layoutGameButton);
    layoutWindow->addLayout(layoutGame);

    // right side - charts
    QVBoxLayout *layoutCharts = new QVBoxLayout();

    // language buttons
    QHBoxLayout *layoutFlags = new QHBoxLayout();

    buttonLanguageEnglish = new QToolButton(this);
    buttonLanguageEnglish->setIcon(QIcon(":/resources/flag_british.png"));
    buttonLanguageEnglish->setIconSize(QSize(48, 30));
    buttonLanguageEnglish->setFixedSize(48, 30);

    buttonLanguagePolish = new QToolButton(this);
    buttonLanguagePolish->setIcon(QIcon(":/resources/flag_polish.png"));
    buttonLanguagePolish->setIconSize(QSize(48, 30));
    buttonLanguagePolish->setFixedSize(48, 30);

    layoutFlags->addStretch();
    layoutFlags->addWidget(buttonLanguageEnglish);
    layoutFlags->addWidget(buttonLanguagePolish);
    layoutCharts->addLayout(layoutFlags);

    // distance chart
    distanceChart = new ChartWidget(tr("Distance"), tr("d [mm]"), 0.0, 500.0, this);
    distanceChart->addSeries(tr("Measured"), Qt::red);
    distanceChart->addSeries(tr("Compensated"), Qt::blue);

    QHBoxLayout *layoutDistCheckboxes = new QHBoxLayout();
    distanceCheckboxes.append(distanceChart->createSeriesCheckbox(0, this));
    distanceCheckboxes.append(distanceChart->createSeriesCheckbox(1, this));
    layoutDistCheckboxes->addWidget(distanceCheckboxes[0]);
    layoutDistCheckboxes->addWidget(distanceCheckboxes[1]);

    layoutCharts->addWidget(distanceChart);
    layoutCharts->addLayout(layoutDistCheckboxes);

    // angle chart
    angleChart = new ChartWidget(tr("Angle"), tr("ang [°]"), -90.0, 90.0, this);
    angleChart->addSeries(tr("Accelerometer"), Qt::red);
    angleChart->addSeries(tr("Gyroscope"), Qt::green);
    angleChart->addSeries(tr("Complementary filter"), Qt::blue);

    QHBoxLayout *layoutAngleCheckboxes = new QHBoxLayout();
    angleCheckboxes.append(angleChart->createSeriesCheckbox(0, this));
    angleCheckboxes.append(angleChart->createSeriesCheckbox(1, this));
    angleCheckboxes.append(angleChart->createSeriesCheckbox(2, this));
    layoutAngleCheckboxes->addWidget(angleCheckboxes[0]);
    layoutAngleCheckboxes->addWidget(angleCheckboxes[1]);
    layoutAngleCheckboxes->addWidget(angleCheckboxes[2]);

    layoutCharts->addWidget(angleChart);
    layoutCharts->addLayout(layoutAngleCheckboxes);

    // chart control buttons
    QHBoxLayout *layoutButtons = new QHBoxLayout();
    QHBoxLayout *layoutChartsButtons = new QHBoxLayout();

    buttonChartsClear = new QPushButton(tr("Clear"), this);
    buttonChartsStopResume = new QPushButton(tr("Stop"), this);
    buttonChartsStopResumeState = false;

    layoutChartsButtons->addWidget(buttonChartsClear);
    layoutChartsButtons->addWidget(buttonChartsStopResume);
    layoutButtons->addLayout(layoutChartsButtons);

    // serial connection button and combobox
    QHBoxLayout *layoutSerial = new QHBoxLayout();

    comboSerialPorts = new QComboBox(this);
    comboSerialPorts->addItems(serialHandler->availablePorts());
    buttonSerialConnectDisconnect = new QPushButton(tr("Connect"), this);

    layoutSerial->addWidget(comboSerialPorts);
    layoutSerial->addWidget(buttonSerialConnectDisconnect);
    layoutButtons->addLayout(layoutSerial);
    layoutCharts->addLayout(layoutButtons);

    layoutWindow->addLayout(layoutCharts);

    // equal stretch for both halves of the window
    layoutWindow->setStretch(0, 1);
    layoutWindow->setStretch(1, 1);

    // signal to slots connections
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
}

void MainWindow::onChartsClearClicked()
{
    distanceChart->clearChart();
    angleChart->clearChart();
}

void MainWindow::onSerialConnectDisconnectClicked()
{
    if (buttonSerialConnectDisconnectState)
    {
        serialHandler->closePort();
        buttonSerialConnectDisconnect->setText(tr("Connect"));
        comboSerialPorts->setEnabled(true);
        comboSerialPorts->clear();
        comboSerialPorts->addItems(serialHandler->availablePorts());
        buttonSerialConnectDisconnectState = false;
    }
    else
    {
        // refresh port list before connecting
        QString selectedPort = comboSerialPorts->currentText();

        if (selectedPort.isEmpty())
        {
            return;
        }

        if (serialHandler->openPort(selectedPort))
        {
            buttonSerialConnectDisconnect->setText(tr("Disconnect"));
            comboSerialPorts->setEnabled(false);
            buttonSerialConnectDisconnectState = true;
        }
    }
}

void MainWindow::onSerialDataReceived(float measuredDist, float realDist, float accDeg, float gyroDeg, float angleDeg)
{
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
    }

    retranslateUi();
}

void MainWindow::retranslateUi()
{
    // window title
    setWindowTitle(tr("AeroDrive2D"));

    // game button
    if (buttonGameStartRestartState)
    {
        buttonGameStartRestart->setText(tr("Restart"));
    }
    else
    {
        buttonGameStartRestart->setText(tr("Start"));
    }

    // chart control buttons
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

    // distance chart and checkboxes
    distanceChart->retranslate(tr("Distance"), tr("d [mm]"), {tr("Measured"), tr("Compensated")});
    QStringList distanceCheckboxesNames = {tr("Measured"), tr("Compensated")};
    for (int i = 0; i < distanceCheckboxes.size(); ++i)
    {
        distanceCheckboxes[i]->setText(distanceCheckboxesNames[i]);
    }

    // angle chart and checkboxes
    angleChart->retranslate(tr("Angle"), tr("ang [°]"), {tr("Accelerometer"), tr("Gyroscope"), tr("Complementary filter")});
    QStringList angleCheckboxesNames = {tr("Accelerometer"), tr("Gyroscope"),  tr("Complementary filter")};
    for (int i = 0; i < angleCheckboxes.size(); ++i)
    {
        angleCheckboxes[i]->setText(angleCheckboxesNames[i]);
    }
}