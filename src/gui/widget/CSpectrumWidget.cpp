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
#include "CSpectrumWidget.h"
#include <QDebug>

CSpectrumWidget::CSpectrumWidget(QWidget *parent) :
    QWidget(parent)
  ,x1(0)
  ,x2(0)
  ,line(0)
  ,average(NULL)
  ,picker(NULL)
  ,refreshrate(20)
{
    setupUi(this);
    spectro = new QwtPlotCurve();

    Background * background;
    spectro->attach(qwtPlot);
    spectro->setPen(QPen(QColor(Qt::green)));
    //spectro->setStyle(QwtPlotCurve::Sticks);
    background = new Background();
    background->attach(qwtPlot);
    //connect(spectro, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotClicked(QPoint)));
    rasterData =new WaterfallData();
    rasterArray=new double[(int)FFTSIZE*WATERFALL_MAX]; // waterfall size is FFT usable bin * 2 for both channels so 512 * height
    rasterData->setRangeX(0.0,FFTSIZE);
    rasterData->setRangeY(1.0,WATERFALL_MAX);

    setPlotterType(eScope);
}

void CSpectrumWidget::slotRawSamples(double *xval, double *yval,int size)
{
    if (average == NULL) {
        average = new double[size];
        // Faster
        memset(average,0,size*sizeof(double));
        /*
        for (int i=0; i< size; i++)
            average[i] = 0.0;
        */
        time.start();
    }

    if (time.elapsed() < refreshrate) {
        if ((plotterType == eWaterfall) || (plotterType == eFFT)) {
            for (int i=0; i<size;i++) {
                // Peak average
                if (yval[i] > average[i])
                    average[i] = (average[i] + yval[i]) / 2.0;
                else
                    average[i] = yval[i];
            }
        }
    } else {
        // Update X axis if size is !=
        if (plotterType == eWaterfall)
        {
            // If spectro is full
            // translate all data
            if (line>WATERFALL_MAX-1) {
                line = line-1;
                // Faster
                memmove(&rasterArray[0],&rasterArray[size],((size*WATERFALL_MAX-1) - size)*sizeof(double));
            }
            // save line by line so
            //Faster
            memcpy(&rasterArray[line * size],&average[0], size*sizeof(double));
            rasterData->setData(rasterArray,FFTSIZE,WATERFALL_MAX);
            waterfall->setData(rasterData);
            line++;
        } else {
            if (x2 != size)
                qwtPlot->setAxisScale(QwtPlot::xBottom,0,size);
            if (plotterType == eScope)
                spectro->setRawSamples(xval,yval,size);
            else
                spectro->setRawSamples(xval,average,size);
        }
        qwtPlot->replot();
        time.restart();
    }
}

void CSpectrumWidget::setupUi(QWidget *widget)
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(200);
    sizePolicy.setVerticalStretch(80);
    sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
    widget->setSizePolicy(sizePolicy);
    widget->setAutoFillBackground(false);
    hboxLayout = new QHBoxLayout(widget);
    qwtPlot = new QwtPlot(widget);
    qwtPlot->setSizePolicy(sizePolicy);
    xScaleDraw = new TimeScaleDraw();
    yScaleDraw = new TimeScaleDraw();
    //qwtPlot->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    qwtPlot->setAxisScaleDraw( QwtPlot::xBottom, xScaleDraw);
    qwtPlot->setAxisScaleDraw( QwtPlot::yLeft, yScaleDraw);
    qwtPlot->setAxisFont(QwtPlot::xBottom, QFont("Helvetica",8,3));
    qwtPlot->setAxisFont(QwtPlot::yLeft, QFont("Helvetica",8,3));
    qwtPlot->setAxisLabelRotation(QwtPlot::xBottom, 35.0);
    hboxLayout->addWidget(qwtPlot);
    qwtPlot->setMouseTracking(true);
    qwtPlot->canvas()->setMouseTracking(true);
    /*
    zoomer = new AdvPlotZoomer(qwtPlot->canvas());
    zoomer->setZoomConstrain(AdvPlotZoomer::HZoomOnly);
    zoomer->setRubberBand(AdvPlotZoomer::VLineRubberBand);
    */
    // Choose a default picker
    setPickerType(eNoPicker);
    qwtPlot->setAutoReplot(false);
    marker = new QwtPlotMarker();
}

void CSpectrumWidget::setAxis(int x1, int x2, int y1, int y2)
{
    this->x1 = x1;
    this->x2 = x2;
    qwtPlot->setAxisScale(QwtPlot::xBottom,x1,x2);
    qwtPlot->setAxisScale(QwtPlot::yLeft,y1,y2);
    xScaleDraw->setParams(SAMPLERATE,FFTSIZE,refreshrate);
}

void CSpectrumWidget::setPlotterType(ePlotter type)
{
    if (plotterType == eWaterfall)
          waterfall->detach();
    plotterType = type;
    switch(type) {
        case eFFT:
                spectro->detach();
                qwtPlot->setAxisTitle(QwtPlot::yLeft, "Power [Db]");
                yScaleDraw->setType(TimeScaleDraw::ePower);
                xScaleDraw->setParams(SAMPLERATE,FFTSIZE,refreshrate);
                xScaleDraw->setType(TimeScaleDraw::eFrequency);
                spectro->attach(qwtPlot);
                qwtPlot->plotLayout()->setAlignCanvasToScales(true);
                qwtPlot->replot();
            break;
        case eScope:
                 spectro->detach();
                 qwtPlot->setAxisTitle(QwtPlot::yLeft, "Power [Db]");
                 yScaleDraw->setType(TimeScaleDraw::ePower);
                 xScaleDraw->setType(TimeScaleDraw::eTime);
                 qwtPlot->plotLayout()->setAlignCanvasToScales(true);
                 spectro->attach(qwtPlot);
                 qwtPlot->replot();
            break;
        case eWaterfall:
                 spectro->detach();
                 waterfall = new QwtPlotSpectrogram;
                 waterfall->setDisplayMode(QwtPlotSpectrogram::ImageMode);
                 waterfall->setData(rasterData);
                 waterfall->setRenderThreadCount(0);
                 waterfall->setCachePolicy(QwtPlotSpectrogram::NoCache);
                 initColorMap(eNormal);
                 initColorBar();
                 qwtPlot->setAxisTitle(QwtPlot::yLeft, "Elapsed time [s]");
                 yScaleDraw->setType(TimeScaleDraw::eTimeInv);
                 xScaleDraw->setParams(SAMPLERATE,FFTSIZE,refreshrate);
                 xScaleDraw->setType(TimeScaleDraw::eFrequency);
                 waterfall->attach(qwtPlot);
                 qwtPlot->plotLayout()->setAlignCanvasToScales(true);
                 qwtPlot->replot();
             break;
        default:
                spectro->detach();
                spectro->attach(qwtPlot);
            break;
    }
}

void CSpectrumWidget::setScaleType(ScaleType type)
{
    xScaleDraw->setType((TimeScaleDraw::ScaleType)type);
}

void CSpectrumWidget::slotClicked(QPointF point)
{
    qDebug() << "slotClicked point=" << point;
    double value = 0.0;
    marker->detach();
    marker->setLinePen(QPen(Qt::red));
    if (_type == eThresholdPicker) {
        marker->setLineStyle(QwtPlotMarker::HLine);
        value = point.y();
        marker->setYValue(value);
        emit threshold(value);
    } else {
        marker->setLineStyle(QwtPlotMarker::VLine);
        value = point.x();
        marker->setXValue(value);
        emit frequency(value*SAMPLERATE/FFTSIZE);
    }
    marker->attach(qwtPlot);
}

void CSpectrumWidget::slotBandWidth(double bw)
{
    //qDebug() << "slotBandWidth bandwidth=" << bw;
    emit bandwidth(bw*SAMPLERATE/FFTSIZE);
}

void CSpectrumWidget::initColorMap(eColorMap colorMap)
{
        switch ((eColorMap)colorMap)
        {
                case eNormal:
                        m_colorMap = new QwtLinearColorMap(Qt::darkCyan, Qt::red);
                        m_colorMap->addColorStop(0.0, Qt::black);
                        m_colorMap->addColorStop(0.05, Qt::blue);
                        m_colorMap->addColorStop(0.25, Qt::yellow);
                        m_colorMap->addColorStop(0.95, Qt::red);
                        break;
                case eGray:
                    m_colorMap = new QwtLinearColorMap(Qt::black, Qt::white);
                        break;
                case eColor:
                        double pos;
                        m_colorMap = new QwtLinearColorMap(QColor(0,0,189), QColor(132,0,0));
                        pos = 1.0/13.0*1.0; m_colorMap->addColorStop(pos, QColor(0,0,255));
                        pos = 1.0/13.0*2.0; m_colorMap->addColorStop(pos, QColor(0,66,255));
                        pos = 1.0/13.0*3.0; m_colorMap->addColorStop(pos, QColor(0,132,255));
                        pos = 1.0/13.0*4.0; m_colorMap->addColorStop(pos, QColor(0,189,255));
                        pos = 1.0/13.0*5.0; m_colorMap->addColorStop(pos, QColor(0,255,255));
                        pos = 1.0/13.0*6.0; m_colorMap->addColorStop(pos, QColor(66,255,189));
                        pos = 1.0/13.0*7.0; m_colorMap->addColorStop(pos, QColor(132,255,132));
                        pos = 1.0/13.0*8.0; m_colorMap->addColorStop(pos, QColor(189,255,66));
                        pos = 1.0/13.0*9.0; m_colorMap->addColorStop(pos, QColor(255,255,0));
                        pos = 1.0/13.0*10.0; m_colorMap->addColorStop(pos, QColor(255,189,0));
                        pos = 1.0/13.0*12.0; m_colorMap->addColorStop(pos, QColor(255,66,0));
                        pos = 1.0/13.0*13.0; m_colorMap->addColorStop(pos, QColor(189,0,0));
                        break;
        }
        waterfall->setColorMap(m_colorMap);
}

void CSpectrumWidget::initColorBar()
{
    QwtScaleWidget *m_rightAxis = qwtPlot->axisWidget(QwtPlot::yRight);
    m_rightAxis->setTitle(tr("Noise Ratio"));
    m_rightAxis->setColorBarEnabled(true);
    m_rightAxis->setColorMap(QwtInterval(1.0,10.0), m_colorMap);

    qwtPlot->setAxisScale(QwtPlot::yRight, 1.0, 10.0 );
    qwtPlot->enableAxis(QwtPlot::yRight);

}

void CSpectrumWidget::setRefreshRate(int milliseconds)
{
    refreshrate = milliseconds;
    yScaleDraw->setParams(SAMPLERATE,FFTSIZE,milliseconds);
    qwtPlot->updateAxes();
    qwtPlot->replot();
}

void CSpectrumWidget::setPickerType(ePickerType type)
{
    if (picker != NULL)
        delete picker;
    switch(type) {
        case eCwPicker:
                picker = new CwPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::UserRubberBand, QwtPicker::AlwaysOn, (QwtPlotCanvas*)qwtPlot->canvas() );
                connect(picker, SIGNAL(selected(QPointF)), this, SLOT(slotClicked(QPointF)));
                connect(picker, SIGNAL(bandwidthChanged(double)), this, SLOT(slotBandWidth(double)));
                qDebug() << "CW Picker";
            break;
        case eRttyPicker:
                picker = new RttyPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::UserRubberBand, QwtPicker::AlwaysOn, (QwtPlotCanvas*)qwtPlot->canvas() );
                connect(picker, SIGNAL(selected(QPointF)), this, SLOT(slotClicked(QPointF)));
                connect(picker, SIGNAL(bandwidthChanged(double)), this, SLOT(slotBandWidth(double)));
                qDebug() << "Rtty Picker";
            break;
        case eThresholdPicker:
                picker = new ThresholdPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::HLineRubberBand, QwtPicker::AlwaysOn, (QwtPlotCanvas*)qwtPlot->canvas() );
                connect(picker, SIGNAL(selected(QPointF)), this, SLOT(slotClicked(QPointF)));
                qDebug() << "Threshold Picker";
            break;
        case eNoPicker:
        default:
            break;
    }
    _type = type;
}
