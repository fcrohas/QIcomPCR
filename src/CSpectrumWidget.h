#ifndef CSPECTRUMWIDGET_H
#define CSPECTRUMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QPainter>
#include <QTime>
#include <QHBoxLayout>
#include <qwt_plot_layout.h>
#include <qwt_plot_curve.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_compat.h>
#include <qwt_picker_machine.h>

class TimeScaleDraw: public QwtScaleDraw
{
public:
    TimeScaleDraw( const QTime &base ):
        baseTime( base )
    {
    }
    virtual QwtText label( double v ) const
    {
        QTime upTime = baseTime.addSecs( ( int )v );
        return upTime.toString();
    }
private:
    QTime baseTime;
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

        for ( int i = 255; i > 0; i -= 10 )
        {
            r.setBottom( yMap.transform( i - 10 ) );
            r.setTop( yMap.transform( i ) );
            painter->fillRect( r, c );

            c = c.dark( 110 );
        }
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

        }

    signals:
        void mouseMoved(const QPoint& pos) const;

    protected:
        QwtText trackerText (const QwtDoublePoint & pos)
        {
            const QPoint point = pos.toPoint();
            emit mouseMoved(point);
            return QwtText(QString::number(point.x()) + ", " + QString::number(point.y()));
        }

};


class CSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSpectrumWidget(QWidget *parent = 0);
    void setAxis(int x1, int x2, int y1, int y2);
signals:
    void frequency(double value);
    
public slots:
    void slotRawSamples(double *xval, double *yval,int size);
    void slotClicked(QPointF point);
private:
    void setupUi(QWidget *widget);
    QwtPlotCurve *spectro;
    QHBoxLayout *hboxLayout;
    QwtPlot *qwtPlot;
    MyPicker *picker;
    int x1;
    int x2;

    
};

#endif // CSPECTRUMWIDGET_H
