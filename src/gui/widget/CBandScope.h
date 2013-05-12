#ifndef CBANDSCOPE_H
#define CBANDSCOPE_H

#include <QWidget>
#include <QHBoxLayout>
#include <qwt_plot.h>
#include <qwt_plot_histogram.h>
#include <qwt_series_data.h>

class CBandScope : public QWidget
{
    Q_OBJECT
public:
    explicit CBandScope(QWidget *parent = 0);
    void setSamples(QString data);
    void setSamples(QVector<QwtIntervalSample> samples);
    void setBandWidth(int size);
    void setStep(int step);
    void setCentralFrequency(int value);
signals:
    
public slots:

private:
    // Ui setup
    void setupUi(QWidget *widget);

    //
    QHBoxLayout *hboxLayout;
    QwtPlotHistogram *bandscope;
    QwtPlot *qwtPlot;
    QVector<QwtIntervalSample> samples;

    // internal
    int size;
    int step;
    int position;
    int frequency;
};

#endif // CBANDSCOPE_H
