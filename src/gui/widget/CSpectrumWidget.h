#ifndef CSPECTRUMWIDGET_H
#define CSPECTRUMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTime>
#include <QHBoxLayout>
#include <QWheelEvent>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_compat.h>
#include <qwt_picker_machine.h>


class MyZoomer : public QwtPlotZoomer
{
    public:
    MyZoomer(QwtPlotCanvas *canvas):
    QwtPlotZoomer(canvas)
    {
        setRubberBandPen(QPen(Qt::darkGreen, 1.5, Qt::SolidLine));
        setRubberBand(QwtPlotZoomer::RectRubberBand);
        setTrackerMode(QwtPicker::AlwaysOn);
        setEnabled(true);
    }

    virtual QwtText trackerTextF(const QPointF &pos) const
    {
        QColor bg(Qt::white);
        QwtText text = QwtPlotZoomer::trackerTextF(pos);
        text.setBackgroundBrush( QBrush( bg ));
        return text;
    }

    virtual void rescale()
    {
        QwtPlotZoomer::rescale();
    }
};

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw()
    {
    }
    virtual QwtText label( double v ) const
    {
        double freq = 0.0;
        if (v <257)
            freq = ((int)v) * 22050.0/512.0;
        else
            freq = ((int)v-256) * 22050.0/512.0;
        return QString(" %1 Hz").arg(freq);
    }
private:
};

class PowerScaleDraw: public QwtScaleDraw
{
public:
    PowerScaleDraw()
    {
    }
    virtual QwtText label( double v ) const
    {
        double power = 10*log(v);
        return QString(" %1 ").arg(power);
    }
private:
};


class Background: public QwtPlotItem
{
public:
    Background()
    {
        setZ( 0.0 );
    }

    virtual int rtti() const
    {
        return QwtPlotItem::Rtti_PlotUserItem;
    }

    virtual void draw( QPainter *painter,
        const QwtScaleMap &, const QwtScaleMap &yMap,
        const QRectF &canvasRect ) const
    {
        QColor c( Qt::white );
        QRectF r = canvasRect;
        painter->fillRect(r, Qt::gray);
/*
        for ( int i = 255; i > 0; i -= 10 )
        {
            r.setBottom( yMap.transform( i - 10 ) );
            r.setTop( yMap.transform( i ) );
            painter->fillRect( r, c );

            c = c.dark(120);
        }
*/
    }
};


class CpuCurve: public QwtPlotCurve
{
public:
    CpuCurve( const QString &title ):
        QwtPlotCurve( title )
    {
        setRenderHint( QwtPlotItem::RenderAntialiased );
    }

    void setColor( const QColor &color )
    {
        QColor c = color;
        c.setAlpha( 150 );

        setPen( c );
        setBrush( c );
    }
};

class MyPicker : public QwtPlotPicker
{
        Q_OBJECT

    public:
        MyPicker( int xAxis, int yAxis, RubberBand rubberBand, DisplayMode trackerMode, QwtPlotCanvas *canvas ):
            QwtPlotPicker(xAxis, yAxis, rubberBand, trackerMode,canvas)
        {
            setTrackerMode( trackerMode );
            setStateMachine( new QwtPickerTrackerMachine() );
            setRubberBand( rubberBand );
            setRubberBandPen(QPen(QColor(Qt::red)));
            bandwidth = 3;
        }

        QwtText trackerText (const QPoint & point) const
        {
            //const QPoint point = pos.toPoint();
            int x = invTransform(point).x();
            double freq = 0.0;
            if (x <257)
                freq = x * 22050.0/512.0;
            else
                freq = (x-256) * 22050.0/512.0;
            return QwtText(QString(" %1 Hz bw=%2 Hz").arg(freq).arg(bandwidth * 22050.0/512.0));
            //return QwtText(QString::number(point.x()) + ", " + QString::number(point.y()));
        }

        void drawRubberBand(QPainter *p) const
        {
            p->save();
            int x = this->trackerPosition().x();
            p->setPen(Qt::blue);
            QBrush brush(Qt::SolidPattern);
            brush.setColor(QColor(0,0,180,180));
            //p->setBrush();
            p->fillRect(QRect(x-(bandwidth/2),0,bandwidth,this->canvas()->height()),brush);
            p->setOpacity(1.0);
            //brush.setColor(Qt::red);
            p->setPen(QColor(100,0,0,180));
            p->drawLine(x,0,x,this->canvas()->height());
            p->restore();
        }
    protected:

        void widgetMousePressEvent(QMouseEvent *mouseEvent)
        {
            emit selected(invTransform(mouseEvent->pos()));
        }

        void widgetWheelEvent(QWheelEvent *wheelEvent)
        {
            if (wheelEvent->delta()>0)
                bandwidth += 2;
            else
                bandwidth -= 2;
            // Block limits
            if (bandwidth <3) bandwidth = 3;
            //qDebug() << "bandwidth="<< bandwidth;
            updateDisplay();
            emit bandwidthChanged(bandwidth);
        }

        /*
        void drawTracker(QPainter *p) const
        {
            int x = this->trackerPosition().x();
            p->drawLine(x,0,x,this->plot()->height());
        }*/

    signals:
        void mouseMoved(const QPoint& pos) const;
        void bandwidthChanged(int bandwidth);
    private:
        int bandwidth;

};


class CSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSpectrumWidget(QWidget *parent = 0);
    void setAxis(int x1, int x2, int y1, int y2);
signals:
    void frequency(double value);
    void bandwidth(int value);
    
public slots:
    void slotRawSamples(double *xval, double *yval,int size);
    void slotClicked(QPointF point);
    void slotBandWidth(int bw);
private:
    void setupUi(QWidget *widget);
    QwtPlotCurve *spectro;
    QHBoxLayout *hboxLayout;
    QwtPlot *qwtPlot;
    MyPicker *picker;
    MyZoomer *zoomer;
    int x1;
    int x2;

    
};

#endif // CSPECTRUMWIDGET_H
