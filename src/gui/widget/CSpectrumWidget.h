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
#include <qwt_plot_spectrogram.h>
#include <qwt_raster_data.h>
#include <qwt_color_map.h>
#include <qwt_plot_zoomer.h>
#include <qwt_scale_draw.h>
#include <qwt_scale_widget.h>
#include <qwt_legend.h>
#include <qwt_legend_item.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>
#include <qwt_plot_marker.h>
#include <qwt_compat.h>
#include <qwt_picker_machine.h>
#define WATERFALL_MAX 512

class WaterfallData: public QwtRasterData
{
private:
        double * m_Array;
        double m_minValue;
        double m_maxValue;

        struct structMinMax{
                double min;
                double max;
        };
        structMinMax m_RangeX;
        structMinMax m_RangeY;
        struct structXY{
                double x;
                double y;
        };
        structXY m_DataSize;
        structXY m_RealToArray;

public:
        // Constructor giving back the QwtRasterData Constructor
    WaterfallData(): QwtRasterData()
    {
                m_Array = NULL;
    }

    ~WaterfallData()
    {
                if (m_Array != NULL)
                        delete [] m_Array;
    }

    virtual QwtRasterData *copy() const
    {
        WaterfallData *clone = new WaterfallData();
                clone->setRangeX(m_RangeX.min, m_RangeX.max);
                clone->setRangeY(m_RangeY.min, m_RangeY.max);
                //clone->setBoundingRect(QwtDoubleRect(m_RangeX.min, m_RangeY.min, m_RangeX.max, m_RangeY.max));
                clone->setInterval(Qt::XAxis, QwtInterval( m_RangeX.min, m_RangeX.max));
                clone->setInterval(Qt::YAxis, QwtInterval( m_RangeY.min, m_RangeY.max));
                clone->setInterval(Qt::ZAxis, QwtInterval(0.0, 30.0));
                clone->setData(m_Array, m_DataSize.x, m_DataSize.y);
                return clone;
    }
/*
    virtual QwtInterval range() const
    {
        qDebug() << "Interval " << QwtInterval(m_minValue, m_maxValue);
        return QwtInterval(0.0, 2.0);
    }
*/
    virtual double value(double x, double y) const
    {
                int xpos = (int)((x - m_RangeX.min) / m_RealToArray.x);
                int ypos = (int)((y - m_RangeY.min) / m_RealToArray.y);
                int pos = ArrPos(xpos, ypos);
                double dvalue = m_Array[pos];
                //qDebug() << "Value = " << dvalue;
                return dvalue;
    }

    void setData(double * Array, int sizex, int sizey)
    {
            m_DataSize.x = sizex;
            m_DataSize.y = sizey;
            int size = sizex * sizey;
            setInterval( Qt::ZAxis, QwtInterval( 0.0,30.0));
            //if (m_Array != NULL)
            //        delete [] m_Array;
            //m_Array = new double [size];
            if (m_Array == NULL)
                m_Array = new double [size];
            memcpy(m_Array, Array, size * sizeof(double));
            //qDebug() << "m_RangeX.min is " << m_RangeX.min << " m_RangeX.max is " << m_RangeX.max;
            //qDebug() << "m_RangeY.min is " << m_RangeY.min << " m_RangeY.max is " << m_RangeY.max;
            //qDebug() << "m_DataSize.x is " << m_DataSize.x << " m_DataSize.y is " << m_DataSize.y;
            m_RealToArray.x = (m_RangeX.max - m_RangeX.min) / (m_DataSize.x - 1);
            m_RealToArray.y = (m_RangeY.max - m_RangeY.min) / (m_DataSize.y - 1);
            //qDebug() << "m_RealToArray.x is " << m_RealToArray.x << " m_RealToArray.y is " << m_RealToArray.y;
    }

    void setRangeX(const double min, const double max)
    {
            m_RangeX.min = min;
            m_RangeX.max = max;
            setInterval( Qt::XAxis, QwtInterval( min, max ) );
    }

    void setRangeY(const double min, const double max)
    {
            m_RangeY.min = min;
            m_RangeY.max = max;
            setInterval( Qt::YAxis, QwtInterval( min, max ) );
    }

    int ArrPos(const int x, const int y) const
    {
            return x + m_DataSize.x * y;
            //return y + m_DataSize.y * x;
    }

};

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

    enum ScaleType {eFrequency=1, eTime=2, eTimeInv=3, ePower=4};

    TimeScaleDraw()
    {
        // Frequency graph by default
        samplerate=22050;
        FftBins=512;
        rate = 10;
        type=eTime;
    }
    QwtText label( double v ) const
    {
        double display = 0.0;
        QString unit("");
        qDebug() << "type label=" << type;
        switch(type) {
            case eFrequency:
                if (v <(FftBins/2)+1)
                    display = ((int)v) * samplerate/FftBins;
                else
                    display = ((int)v-(FftBins/2)) * samplerate/FftBins;
                unit = "Hz";
                break;
            case eTime:
                display = v * 1000/samplerate;
                unit = "ms";
                break;
            case eTimeInv:
                display = abs(v-512) * 0.010;
                unit = "s";
                break;
            case ePower:
                display = 10*log(v);
                unit = "Db";
                break;
            default:
                break;
        }
        return QString(" %1 %2").arg(display).arg(unit);
    }

    void setType(ScaleType value) {
        type = value;
        qDebug() << "type setType=" << type;
        //invalidateCache();

    }

    void setParams(uint rate, uint bins, uint refresh) {
        samplerate = rate;
        FftBins = bins;
        rate = refresh;
    }

private:
    ScaleType type;
    uint samplerate;
    uint FftBins;
    uint rate;
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
        painter->fillRect(r, Qt::black);
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
            samplerate=22050;
            FftBins=512;
            bandwidth = 3.0;
        }

        QwtText trackerText (const QPoint & point) const
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
            //return QwtText(QString::number(point.x()) + ", " + QString::number(point.y()));
        }

        void drawRubberBand(QPainter *p) const
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

        void setParams(uint rate, uint bins) {
            samplerate = rate;
            FftBins = bins;
        }

    protected:

        void widgetMousePressEvent(QMouseEvent *mouseEvent)
        {
            emit selected(invTransform(mouseEvent->pos()));
        }

        void widgetWheelEvent(QWheelEvent *wheelEvent)
        {
            if (wheelEvent->delta()>0)
                bandwidth += 0.5;
            else
                bandwidth -= 0.5;
            // Block limits
            if (bandwidth <1.0) bandwidth = 1.0;
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
        void bandwidthChanged(double bandwidth);
    private:
        double bandwidth;
        uint samplerate;
        uint FftBins;

};

class ColorMap: public QwtLinearColorMap
{
public:
    ColorMap():
        QwtLinearColorMap( Qt::darkCyan, Qt::red )
    {
        addColorStop( 0.1, Qt::cyan );
        addColorStop( 0.10, Qt::green );
        addColorStop( 0.35, Qt::yellow );
    }
};


class CSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSpectrumWidget(QWidget *parent = 0);
    enum ePlotter {eFFT=0,eScope=1,eWaterfall=2};
    enum eColorMap {eNormal=1, eGray=2, eColor=3};
    enum ScaleType {eFrequency=1, eTime=2, eTimeInv=3, ePower=4};
    void setAxis(int x1, int x2, int y1, int y2);
    void setScaleType(ScaleType type);
    void setPlotterType(ePlotter type);
    void initColorMap(eColorMap colorMap);

signals:
    void frequency(double value);
    void bandwidth(double value);
    
public slots:
    void slotRawSamples(double *xval, double *yval,int size);
    void slotClicked(QPointF point);
    void slotBandWidth(double bw);
    void setRefreshRate(int milliseconds);

private:
    void setupUi(QWidget *widget);
    void initColorBar();
    QwtPlotCurve *spectro;
    QwtPlotSpectrogram *waterfall;
    QHBoxLayout *hboxLayout;
    QwtPlot *qwtPlot;
    MyPicker *picker;
    MyZoomer *zoomer;
    TimeScaleDraw *xScaleDraw;
    TimeScaleDraw *yScaleDraw;
    QwtPlotMarker *marker;
    QTime time;

    WaterfallData *rasterData;
    double *rasterArray;
    int x1;
    int x2;
    ePlotter plotterType;
    int line;
    QwtLinearColorMap *m_colorMap;
    int refreshrate;
    double *average;
    
};

#endif // CSPECTRUMWIDGET_H