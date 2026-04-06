#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QElapsedTimer>
#include <QList>
#include <QCheckBox>

class ChartWidget: public QChartView
{
public:
    ChartWidget(const QString &chartName, const QString &yAxisName, float yMin, float yMax, QWidget *parent = nullptr);
    void addPoint(int seriesIndex, float value);
    void clearChart();
    void addSeries(const QString &seriesName, const QColor &color);
    QCheckBox* createSeriesCheckbox(int seriesIndex, QWidget *parent = nullptr);
private:
    QChart *chart;
    QList<QLineSeries*> seriesList;
    QValueAxis *xAxis;
    QValueAxis *yAxis;
    QElapsedTimer elapsedTimer;
    float timeWindow;
};

#endif // CHARTWIDGET_H
