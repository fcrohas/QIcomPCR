#ifndef CBANDSCOPE_H
#define CBANDSCOPE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <qwt_plot.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_picker.h>
#include <qwt_picker_machine.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_histogram.h>
#include <qwt_series_data.h>

class FrequencyScaleDraw: public QwtScaleDraw
{
public:

    FrequencyScaleDraw()
    {
        // Frequency graph by default
        stepSize = 100000;
        center = 106500000;
    }
    QwtText label( double v ) const
    {
        double display = 0.0;
        double min = this->scaleMap().s1();
        double max = this->scaleMap().s2();
        double middle = (max - min) / 2.0;
        double size = max-min;
        double minfreq = center - middle * stepSize;
        //qDebug() << "scale 1=" << min << "scale 2=" << max;
        //qDebug() << "center=" << QString("%1").arg(center) << " minfreq=" << QString("%1").arg(minfreq,10);
        // Middle is tuned frequency
        display = minfreq + (v - min) * stepSize;
        //qDebug() << "display=" << display << "step=" << stepSize;
        return QString(" %1 MHz").arg(display/1000);
    }

    void setParams(int centerFreq, int step) {
        center = centerFreq;
        stepSize = step;
    }

private:
    int center;
    int stepSize;
};

class MyBandPicker : public QwtPlotPicker
{
        Q_OBJECT

    public:
        MyBandPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas ):
            QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode,canvas)
        {
            setTrackerMode( trackerMode );
            setStateMachine( new QwtPickerTrackerMachine() );
            setRubberBand( rubberBand );
            setRubberBandPen(QPen(QColor(Qt::red)));
            // Frequency graph by default
            stepSize = 12500;
            center = 106500000;
        }

        void setParams(int centerFreq, int step) {
            center = centerFreq;
            stepSize = step;
        }

    protected:

        void widgetMousePressEvent(QMouseEvent *mouseEvent)
        {
            emit selected(invTransform(mouseEvent->pos()));
        }

    signals:
    private:
        int center;
        int stepSize;
};


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
    void frequencyChanged(QString value);
public slots:
    void slotClicked(QPointF p);
private:
    // Ui setup
    void setupUi(QWidget *widget);
    //
    QHBoxLayout *hboxLayout;
    QwtPlotHistogram *bandscope;
    QwtPlot *qwtPlot;
    QVector<QwtIntervalSample> samples;
    FrequencyScaleDraw *xAxis;
    MyBandPicker *picker;

    // internal
    int size;
    int step;
    int position;
    int frequency;
};

#endif // CBANDSCOPE_H
