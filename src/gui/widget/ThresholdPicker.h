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
#ifndef THRESHOLDPICKER_H
#define THRESHOLDPICKER_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <qwt_plot_picker.h>
#include <qwt_plot_canvas.h>
#include <qwt_picker_machine.h>

class ThresholdPicker : public QwtPlotPicker
{
    Q_OBJECT
public:
    explicit ThresholdPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas );

signals:
    
public slots:

protected:
    void widgetMousePressEvent(QMouseEvent *mouseEvent);
    
};

#endif // THRESHOLDPICKER_H
