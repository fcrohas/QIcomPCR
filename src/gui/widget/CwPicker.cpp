#include "CwPicker.h"

CwPicker::CwPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas ) :
    QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode,canvas)
{
    setTrackerMode( trackerMode );
    setStateMachine( new QwtPickerTrackerMachine() );
    setRubberBand( rubberBand );
    setRubberBandPen(QPen(QColor(Qt::red)));
    samplerate=SAMPLERATE;
    FftBins=FFTSIZE;
    bandwidth = 3.0;
}

QwtText CwPicker::trackerText (const QPoint & point) const
{
    //const QPoint point = pos.toPoint();
    QwtText text;
    text.setColor(Qt::white);
    double x = invTransform(point).x();
    double freq = 0.0;
    if (x <FftBins/2)
        freq = x * samplerate/FftBins;
    else
        freq = (x-(FftBins/2)) * samplerate/FftBins;
    QString freqText(" %1 Hz bw=%2 Hz");
    text.setText(freqText.arg(freq).arg(bandwidth * samplerate/FftBins));
    return text;
}

void CwPicker::drawRubberBand(QPainter *p) const
{
    p->save();
    int x = this->trackerPosition().x();
    p->setPen(Qt::blue);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(QColor(0,0,120,180));
    //p->setBrush();
    p->fillRect(QRectF(x-(bandwidth/2.0),0,bandwidth,this->canvas()->height()),brush);
    p->setOpacity(1.0);
    //brush.setColor(Qt::red);
    p->setPen(QColor(180,0,0,180));
    p->drawLine(x,0,x,this->canvas()->height());
    p->restore();
}

void CwPicker::setParams(uint rate, uint bins)
{
    samplerate = rate;
    FftBins = bins;
}

void CwPicker::widgetWheelEvent(QWheelEvent *wheelEvent)
{
    if (wheelEvent->delta()>0)
        bandwidth += 0.5;
    else
        bandwidth -= 0.5;
    // Block limits
    if (bandwidth <1.0) bandwidth = 1.0;
    updateDisplay();
    emit bandwidthChanged(bandwidth);
}

void CwPicker::widgetMousePressEvent(QMouseEvent *mouseEvent)
{
    emit selected(invTransform(mouseEvent->pos()));
}
