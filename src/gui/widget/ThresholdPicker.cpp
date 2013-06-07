#include "ThresholdPicker.h"

ThresholdPicker::ThresholdPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas ) :
    QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode,canvas)
{
    setTrackerMode( trackerMode );
    setStateMachine( new QwtPickerTrackerMachine() );
    setRubberBand( rubberBand );
    setRubberBandPen(QPen(QColor(Qt::red)));
}

void ThresholdPicker::widgetMousePressEvent(QMouseEvent *mouseEvent)
{
    emit selected(invTransform(mouseEvent->pos()));
}
