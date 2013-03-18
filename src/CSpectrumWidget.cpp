#include "CSpectrumWidget.h"
#include <QDebug>

CSpectrumWidget::CSpectrumWidget(QWidget *parent) :
    QWidget(parent)
{
    setupUi(this);
    spectro = new QwtPlotCurve();

    CpuCurve *curve;
    Background * background;
    spectro->attach(qwtPlot);
    curve = new CpuCurve( "FFT" );
    curve->setColor( Qt::blue );
    curve->attach(qwtPlot);
    background = new Background();
    background->attach(qwtPlot);
}

void CSpectrumWidget::slotRawSamples(double *xval, double *yval,int size)
{
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
    hboxLayout->addWidget(qwtPlot);

    //QMetaObject::connectSlotsByName(widget);
}

void CSpectrumWidget::setAxis(int x1, int x2, int y1, int y2)
{
    qwtPlot->setAxisScale(QwtPlot::xBottom,x1,x2);
    qwtPlot->setAxisScale(QwtPlot::yLeft,y1,y2);
}
