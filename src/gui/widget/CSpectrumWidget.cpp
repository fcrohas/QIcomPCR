#include "CSpectrumWidget.h"
#include <QDebug>

CSpectrumWidget::CSpectrumWidget(QWidget *parent) :
    QWidget(parent)
  ,x1(0)
  ,x2(0)
{
    setupUi(this);
    spectro = new QwtPlotCurve();

    CpuCurve *curve;
    Background * background;
    spectro->attach(qwtPlot);
    spectro->setPen(QPen(QColor(Qt::black)));
    //spectro->setStyle(QwtPlotCurve::Sticks);
    //curve = new CpuCurve( "FFT" );
    //curve->setColor( Qt::blue );
    //curve->attach(qwtPlot);
    background = new Background();
    background->attach(qwtPlot);
    //connect(spectro, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotClicked(QPoint)));
}

void CSpectrumWidget::slotRawSamples(double *xval, double *yval,int size)
{
    // Update X axis if size is !=
    if (x2 != size)
        qwtPlot->setAxisScale(QwtPlot::xBottom,0,size);
    spectro->setRawSamples(xval,yval,size);
    qwtPlot->replot();
}

void CSpectrumWidget::setupUi(QWidget *widget)
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sizePolicy.setHorizontalStretch(200);
    sizePolicy.setVerticalStretch(80);
    sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
    widget->setSizePolicy(sizePolicy);
    widget->setAutoFillBackground(false);
    hboxLayout = new QHBoxLayout(widget);
    qwtPlot = new QwtPlot(widget);
    qwtPlot->setSizePolicy(sizePolicy);
    qwtPlot->setAxisTitle(QwtPlot::yLeft, "Power [Db]");
    qwtPlot->setAxisTitle(QwtPlot::xBottom, "Frequency [Hz]");
    qwtPlot->setAxisScaleDraw( QwtPlot::xBottom, new TimeScaleDraw());
    qwtPlot->setAxisScaleDraw( QwtPlot::yLeft, new PowerScaleDraw());
    qwtPlot->setAxisFont(QwtPlot::xBottom, QFont("Helvetica",8,3));
    qwtPlot->setAxisFont(QwtPlot::yLeft, QFont("Helvetica",8,3));
    qwtPlot->setAxisLabelRotation(QwtPlot::xBottom, 35.0);
    hboxLayout->addWidget(qwtPlot);
    qwtPlot->setMouseTracking(true);
    qwtPlot->canvas()->setMouseTracking(true);
    //zoomer = new MyZoomer(qwtPlot->canvas());
    //picker = new QwtPlotPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOn, qwtPlot->canvas() );
    picker = new MyPicker(QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::UserRubberBand, QwtPicker::AlwaysOn, qwtPlot->canvas() );
    //picker->setStateMachine(new QwtPickerTrackerMachine());
    //picker->setRubberBand(QwtPlotPicker::VLineRubberBand);
    //picker->setRubberBandPen(QPen(QColor(Qt::red)));
    //picker->setTrackerPen(QColor(Qt::red));
    connect(picker, SIGNAL(selected(QPointF)), this, SLOT(slotClicked(QPointF)));
    connect(picker, SIGNAL(bandwidthChanged(int)), this, SLOT(slotBandWidth(int)));

    //QMetaObject::connectSlotsByName(widget);
}

void CSpectrumWidget::setAxis(int x1, int x2, int y1, int y2)
{
    this->x1 = x1;
    this->x2 = x2;
    qwtPlot->setAxisScale(QwtPlot::xBottom,x1,x2);
    qwtPlot->setAxisScale(QwtPlot::yLeft,y1,y2);
}

void CSpectrumWidget::slotClicked(QPointF point)
{
    qDebug() << "point=" << point;
    emit frequency(point.toPoint().x());
}

void CSpectrumWidget::slotBandWidth(int bw)
{
    emit bandwidth(bw);
}
