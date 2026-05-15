/**
 * @file chartwidget.h
 * @brief Declaration of the ChartWidget class.
 *
 * ChartWidget provides a real-time chart based on Qt Charts.
 * It supports multiple named data series with individual colors and visibility toggles.
 */

#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QChartView>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>
#include <QElapsedTimer>
#include <QList>
#include <QCheckBox>

/**
 * @class ChartWidget
 * @brief A time-series chart widget with multi-series support.
 *
 * ChartWidget manages one or more QLineSeries that share a common time axis and value axis.
 * New data points are timestamped using a QElapsedTimer started at construction.
 * The X axis automatically scrolls to show the most recent seconds of data.
 */
class ChartWidget: public QChartView
{
public:
    /**
     * @brief Constructs a ChartWidget with the given axis configuration.
     * @param[in] chartName -- Title displayed above the chart.
     * @param[in] yAxisName -- Label for the vertical axis.
     * @param[in] yMin -- Minimum value of the Y axis.
     * @param[in] yMax -- Maximum value of the Y axis.
     * @param[in,out] parent -- Optional parent widget.
     */
    ChartWidget(const QString &chartName, const QString &yAxisName, float yMin, float yMax, QWidget *parent = nullptr);

    /**
     * @brief Appends a data point to the specified series.
     * @param[in] seriesIndex -- Index of the chosen series.
     * @param[in] value -- Value to append.
     */
    void addPoint(int seriesIndex, float value);

    /**
     * @brief Removes all data points from every series and resets the time axis.
     */
    void clearChart();

    /**
     * @brief Adds a new data series to the chart.
     * @param[in] seriesName -- Series name shown in the legend.
     * @param[in] color -- Line color for added series.
     */
    void addSeries(const QString &seriesName, const QColor &color);

    /**
     * @brief Creates a QCheckBox that toggles visibility of the given series.
     * @param[in] seriesIndex -- Index of the chosen series.
     * @param[in,out] parent -- Optional parent widget for the checkbox.
     * @return Pointer to the newly created QCheckBox.
     */
    QCheckBox* createSeriesCheckbox(int seriesIndex, QWidget *parent = nullptr);

    /**
     * @brief Retranslates the chart.
     * @param[in] title -- Title displayed above the chart.
     * @param[in] yAxisLabel -- Label for the vertical axis.
     * @param[in] seriesNames -- List with series' names.
     */
    void retranslate(const QString &title, const QString &yAxisLabel, const QStringList &seriesNames);

private:
    /**
     *  @brief The QChart instance rendered by this widget.
     */
    QChart *chart;

    /**
     *  @brief List of all data series added to the chart.
     */
    QList<QLineSeries*> seriesList;

    /**
     *  @brief Horizontal (time) axis.
     */
    QValueAxis *xAxis;

    /**
     *  @brief Vertical (value) axis.
     */
    QValueAxis *yAxis;

    /**
     *  @brief Timer used to timestamp incoming data points.
     */
    QElapsedTimer elapsedTimer;

    /**
     *  @brief Width of the visible time window in seconds.
     */
    float timeWindow;
};

#endif // CHARTWIDGET_H
