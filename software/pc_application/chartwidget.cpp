/**
 * @file chartwidget.cpp
 * @brief Implementation of the ChartWidget class.
 */

#include "chartwidget.h"

ChartWidget::ChartWidget(const QString &chartName, const QString &yAxisName,float yMin, float yMax, QWidget *parent): QChartView(parent)
{
    timeWindow = 5.0f;

    // chart configuration
    chart = new QChart();
    chart->setTitle(chartName);
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // time axis
    xAxis = new QValueAxis();
    xAxis->setTitleText("t [s]");
    xAxis->setMin(0);
    xAxis->setMax(timeWindow);
    chart->addAxis(xAxis, Qt::AlignBottom);

    // value axis
    yAxis = new QValueAxis();
    yAxis->setTitleText(yAxisName);
    yAxis->setMin(yMin);
    yAxis->setMax(yMax);
    chart->addAxis(yAxis, Qt::AlignLeft);

    setChart(chart);
    elapsedTimer.start();
}

void ChartWidget::addPoint(int seriesIndex, float value)
{
    if (seriesIndex < 0 || seriesIndex >= seriesList.size())
    {
        return;
    }

    float time = elapsedTimer.elapsed() / 1000.0f;
    seriesList[seriesIndex]->append(time, value);

    // remove all points outside the visible time window
    QList<QPointF> points = seriesList[seriesIndex]->points();
    while(points.size() > 0 && points.first().x() < time - timeWindow)
    {
        points.removeFirst();
    }
    seriesList[seriesIndex]->replace(points);

    // scroll the time axis
    if (time > timeWindow)
    {
        xAxis->setRange(time - timeWindow, time);
    }

}

void ChartWidget::clearChart()
{
    for (QLineSeries *series: seriesList)
    {
        series->clear();
    }

    elapsedTimer.restart();
    xAxis->setRange(0, timeWindow);
}

void ChartWidget::addSeries(const QString &seriesName, const QColor &color)
{
    QLineSeries *series = new QLineSeries();
    series->setName(seriesName);
    series->setColor(color);
    chart->addSeries(series);
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);
    seriesList.append(series);
}

QCheckBox* ChartWidget::createSeriesCheckbox(int seriesIndex, QWidget *parent)
{
    QLineSeries *series = seriesList[seriesIndex];
    QCheckBox *checkbox = new QCheckBox(series->name(), parent);
    checkbox->setChecked(true);

    connect(checkbox, &QCheckBox::toggled, series, &QLineSeries::setVisible);

    return checkbox;
}

void ChartWidget::retranslate(const QString &title, const QString &yAxisLabel, const QStringList &seriesNames)
{
    chart->setTitle(title);
    yAxis->setTitleText(yAxisLabel);

    for (int i = 0; i < seriesList.size(); ++i)
    {
        seriesList[i]->setName(seriesNames[i]);
    }
}
