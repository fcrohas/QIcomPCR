#include "CBandScope.h"

CBandScope::CBandScope(QWidget *parent) :
    QWidget(parent),
    size(500000),
    step(12500),
    position(128),
    samples(256),
    frequency(106500000)
{
    setupUi(this);
}

void CBandScope::setupUi(QWidget *widget)
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(200);
    sizePolicy.setVerticalStretch(200);
    //sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
    widget->setSizePolicy(sizePolicy);
    widget->setAutoFillBackground(false);
    hboxLayout = new QHBoxLayout(widget);
    qwtPlot = new QwtPlot(widget);
    qwtPlot->setSizePolicy(sizePolicy);
    qwtPlot->setAxisScale(QwtPlot::xBottom,64.0,176.0);
    qwtPlot->setAxisScale(QwtPlot::yLeft,0.0,50.0);
    hboxLayout->addWidget(qwtPlot);
    bandscope = new QwtPlotHistogram("BandScope");
    bandscope->setStyle(QwtPlotHistogram::Columns);
    bandscope->setPen(QPen(Qt::blue));
    bandscope->setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    bandscope->attach(qwtPlot);
    //qwtPlot->replot();
}

void CBandScope::setSamples(QString data)
{
    // Offset of those samples
    bool ok;
    int offset = data.mid(3,2).toInt(&ok,16);
    for(int i=0; i < 16; i++) {
        // update sample at position with new value
        double sample = data.mid(5+i*2,2).toInt(&ok,16);
        samples[offset+i] = QwtIntervalSample(sample,offset+i,offset+i+1);
    }
    // Update samples
    bandscope->setSamples(samples);
    qwtPlot->replot();
}

void CBandScope::setSamples(QVector<QwtIntervalSample> samples)
{
    this->samples = samples;
    bandscope->setSamples(samples);
}

void CBandScope::setBandWidth(int value)
{
    size = value;
    // Adjust scale to new bandwidth
    // 80 hex is middle so
    int min = 128 - (size / step) / 2;
    int max = 128 + (size / step) / 2;
    qwtPlot->setAxisScale(QwtPlot::xBottom,min,max);
}

void CBandScope::setStep(int value)
{
    step = value;
    int min = 128 - (size / step) / 2;
    int max = 128 + (size / step) / 2;
    qwtPlot->setAxisScale(QwtPlot::xBottom,min,max);
}

void CBandScope::setCentralFrequency(int value)
{
    frequency = value;
}
