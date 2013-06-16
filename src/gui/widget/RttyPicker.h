#ifndef RTTYPICKER_H
#define RTTYPICKER_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>
#include <consts.h>


class RttyPicker : public QwtPlotPicker
{
    Q_OBJECT
public:
    RttyPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas );
    QwtText trackerText (const QPoint & point) const;
    void drawRubberBand(QPainter *p) const;
    void setParams(uint rate, uint bins);

protected:

    void widgetMousePressEvent(QMouseEvent *mouseEvent);
    void widgetWheelEvent(QWheelEvent *wheelEvent);

signals:
    void mouseMoved(const QPoint& pos) const;
    void bandwidthChanged(double bandwidth);
private:
    double bandwidth;
    uint samplerate;
    uint FftBins;
};

#endif // RTTYPICKER_H
