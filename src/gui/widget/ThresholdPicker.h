#ifndef THRESHOLDPICKER_H
#define THRESHOLDPICKER_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <qwt_plot_picker.h>
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
