#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

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

private slots:
    void onGameStartRestartClicked();
    void onChartsStopResumeClicked();
    void onChartsClearClicked();

};

#endif // MAINWINDOW_H