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
#include "RttyPicker.h"

RttyPicker::RttyPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas ) :
    QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode,canvas)
{
    setTrackerMode( trackerMode );
    setStateMachine( new QwtPickerTrackerMachine() );
    setRubberBand( rubberBand );
    setRubberBandPen(QPen(QColor(Qt::red)));
    samplerate=SAMPLERATE;
    FftBins=FFTSIZE;
    bandwidth = 50.0;
}

QwtText RttyPicker::trackerText (const QPoint & point) const
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
    QString freqText(" %1 Hz space is %2 Hz");
    text.setText(freqText.arg(freq).arg(bandwidth * samplerate/FftBins));
    return text;
}

void RttyPicker::drawRubberBand(QPainter *p) const
{
    p->save();
    int x = this->trackerPosition().x();
    p->setPen(Qt::green);
    p->drawLine(x,0,x,this->canvas()->height());
    p->setPen(Qt::red);
    p->drawLine(x-(bandwidth/2.0),0,x-(bandwidth/2.0),this->canvas()->height());
    p->drawLine(x+(bandwidth/2.0),0,x+(bandwidth/2.0),this->canvas()->height());
    p->restore();
}

void RttyPicker::setParams(uint rate, uint bins)
{
    samplerate = rate;
    FftBins = bins;
}

void RttyPicker::widgetWheelEvent(QWheelEvent *wheelEvent)
{
    if (wheelEvent->delta()>0)
        bandwidth += 5;
    else
        bandwidth -= 5;
    // Block limits
    if (bandwidth <1.0) bandwidth = 1.0;
    updateDisplay();
    emit bandwidthChanged(bandwidth);
}

void RttyPicker::widgetMousePressEvent(QMouseEvent *mouseEvent)
{
    emit selected(invTransform(mouseEvent->pos()));
}
