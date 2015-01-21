/**********************************************************************************************
  Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
#include "CBandScope.h"

CBandScope::CBandScope(QWidget *parent) :
    QWidget(parent),
    size(5000000),
    step(100000),
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
    xAxis = new FrequencyScaleDraw();
    qwtPlot = new QwtPlot(widget);
    qwtPlot->setSizePolicy(sizePolicy);
    qwtPlot->enableAxis(QwtPlot::yLeft,false);
    qwtPlot->setAxisAutoScale(QwtPlot::yLeft, false);
    qwtPlot->setAxisScale(QwtPlot::yLeft,0.0,50.0);
    qwtPlot->setCanvasBackground(QBrush(Qt::black));
    qwtPlot->setAxisScaleDraw(QwtPlot::xBottom, xAxis);
    qwtPlot->setAxisScale(QwtPlot::xBottom,0.0,256.0);
    qwtPlot->setAxisLabelRotation(QwtPlot::xBottom, 35.0);
    qwtPlot->setMouseTracking(true);
    qwtPlot->canvas()->setMouseTracking(true);
    picker = new MyBandPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::VLineRubberBand, QwtPicker::AlwaysOn, (QwtPlotCanvas*)qwtPlot->canvas() );
    hboxLayout->addWidget(qwtPlot);
    connect(picker, SIGNAL(selected(QPointF)), this, SLOT(slotClicked(QPointF)));
    bandscope = new QwtPlotHistogram("BandScope");
    bandscope->setStyle(QwtPlotHistogram::Columns);
    bandscope->setPen(QPen(Qt::blue));
    bandscope->setBrush(QBrush(Qt::blue,Qt::SolidPattern));
    bandscope->attach(qwtPlot);
    //qwtPlot->replot();
}

void CBandScope::setSamples(int16_t* data, int length) {
    for (int i=0; i < length; i++) {
        samples[i] = QwtIntervalSample(data[i],i,i+1);
    }
    bandscope->setSamples(samples);
    qwtPlot->replot();
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
    xAxis->setParams(frequency,step);
    qwtPlot->setAxisScale(QwtPlot::xBottom,min,max);
}

void CBandScope::setStep(int value)
{
    step = value;
    int min = 128 - (size / step) / 2;
    int max = 128 + (size / step) / 2;
    xAxis->setParams(frequency,step);
    qwtPlot->setAxisScale(QwtPlot::xBottom,min,max);
    //qwtPlot->axisWidget( QwtPlot::xBottom )->repaint();
}

void CBandScope::setCentralFrequency(int value)
{
    frequency = value;
    int min = 128 - (size / step) / 2;
    int max = 128 + (size / step) / 2;
    xAxis->setParams(frequency,step);
    qwtPlot->setAxisScale(QwtPlot::xBottom,min,max);
    qwtPlot->axisWidget( QwtPlot::xBottom )->repaint();
}

void CBandScope::slotClicked(QPointF p)
{
    double min = qwtPlot->axisWidget(QwtPlot::xBottom)->scaleDraw()->scaleMap().s1();
    double max = qwtPlot->axisWidget(QwtPlot::xBottom)->scaleDraw()->scaleMap().s2();
    double middle = (max - min) / 2.0;
    double size = max-min;
    double minfreq = frequency - middle * step;
    // Middle is tuned frequency
    //emit fre minfreq + v * stepSize;
    frequency = minfreq + (p.toPoint().x()-min) * step;
    emit frequencyChanged(QString("%1").arg(frequency));
    setCentralFrequency(frequency);
}
