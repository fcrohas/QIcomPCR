#ifndef CSPECTRUMWIDGET_H
#define CSPECTRUMWIDGET_H

#include <QObject>
#include <QWidget>
#include <QHBoxLayout>
#include "qwt_plot_curve.h"
#include "qwt_plot.h"

class CSpectrumWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CSpectrumWidget(QWidget *parent = 0);
    void setAxis(int x1, int x2, int y1, int y2);
signals:
    
public slots:
    void slotRawSamples(double *xval, double *yval,int size);
private:
    void setupUi(QWidget *widget);
    QwtPlotCurve *spectro;
    QHBoxLayout *hboxLayout;
    QwtPlot *qwtPlot;

    
};

#endif // CSPECTRUMWIDGET_H
