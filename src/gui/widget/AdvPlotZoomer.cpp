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
#include "AdvPlotZoomer.h"

AdvPlotZoomer::AdvPlotZoomer( QwtPlotCanvas *canvas, bool doReplot )
    : QwtPlotZoomer( canvas, doReplot ),
      d_zoomConstrain( NoConstrain )
{
    // Nothing to be done here
}

AdvPlotZoomer::AdvPlotZoomer( int xAxis, int yAxis, QwtPlotCanvas *canvas,
    bool doReplot )
    : QwtPlotZoomer( xAxis, yAxis, canvas, doReplot ),
      d_zoomConstrain( NoConstrain )
{
    // Nothing to be done here
}

AdvPlotZoomer::~AdvPlotZoomer()
{
    // Nothing to be done here
}

void AdvPlotZoomer::drawRubberBand( QPainter *painter ) const
{
    if ( !isActive() || rubberBandPen().style() == Qt::NoPen )
        return;

    QwtPickerMachine::SelectionType selectionType =
        QwtPickerMachine::NoSelection;
    if ( stateMachine() )
        selectionType = stateMachine()->selectionType();

    if ( d_zoomConstrain == NoConstrain || rubberBand() != RectRubberBand ) {
        QwtPlotZoomer::drawRubberBand(painter);
        return;
    }

    const QRect &pRect = pickRect();
    const QPolygon &pa = pickedPoints();
    const int npa      = pa.count();

    switch ( d_zoomConstrain ) {
    case HZoomOnly:
        if ( npa >= 1 )
            QwtPainter::drawLine(painter, pa[0].x(), pRect.top(), pa[0].x(),
                pRect.bottom());
        if ( npa >= 2 )
            QwtPainter::drawLine(painter, pa[1].x(), pRect.top(), pa[1].x(),
                pRect.bottom());
        break;
    case VZoomOnly:
        if ( npa >= 1 )
            QwtPainter::drawLine(painter, pRect.left(), pa[0].y(),
                pRect.right(), pa[0].y());
        if ( npa >= 2 )
            QwtPainter::drawLine(painter, pRect.left(), pa[1].y(),
                pRect.right(), pa[1].y());
        break;
    default:
        // Nothing to be done here
        break;
    }
}

AdvPlotZoomer::ZoomConstrain AdvPlotZoomer::zoomConstrain() const
{
    return d_zoomConstrain;
}

void AdvPlotZoomer::setZoomConstrain( ZoomConstrain zoomConstrain )
{
    if ( d_zoomConstrain == zoomConstrain )
        return;
    d_zoomConstrain = zoomConstrain;
    if ( zoomConstrain == NoConstrain || !isActive() )
        return;
    /// \todo Implement changing the zoomConstrain during a zooming selection
}

bool AdvPlotZoomer::accept( QPolygon &pa ) const
{
    if ( !QwtPlotZoomer::accept(pa) )
        return false;
    if ( d_zoomConstrain == NoConstrain )
        return true;

    QRect zoomBaseInt = transform(zoomBase());
    switch ( d_zoomConstrain ) {
    case HZoomOnly:
        pa[0].setY(zoomBaseInt.top());
        pa[1].setY(zoomBaseInt.bottom());
        break;
    case VZoomOnly:
        pa[0].setX(zoomBaseInt.left());
        pa[1].setX(zoomBaseInt.right());
        break;
    default:
        // Nothing to be done here
        break;
    }
    return true;
}
