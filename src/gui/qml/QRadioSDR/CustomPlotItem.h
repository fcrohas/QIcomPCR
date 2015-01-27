#pragma once

#include <QtQuick>
#include "qcustomplot.h"
class QCustomPlot;

class CustomAxis : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label WRITE setlabel)
    Q_PROPERTY(QColor color READ color WRITE setcolor)
    Q_PROPERTY(qreal min READ min WRITE setMin)
    Q_PROPERTY(qreal max READ max WRITE setMax)

public:
    // Axis Label
    QString label() const { return m_label; }
    void setlabel(QString label) { m_label = label;}
    // Axis color
    QColor color() const { return m_color; }
    void setcolor(QColor color) { m_color = color;}
    // Min axis value
    qreal min() const { return m_min; }
    void setMin(qreal value) { m_min = value; }

    // Max axis value
    qreal max() const { return m_max; }
    void setMax(qreal value) { m_max = value; }

private:
    QColor m_color;
    QString m_label;
    qreal m_min;
    qreal m_max;

};

class CustomPlotItem : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(QColor background READ background WRITE setBackground)
    Q_PROPERTY(CustomAxis* yAxis READ yAxis WRITE setyAxis)
    Q_PROPERTY(CustomAxis* xAxis READ xAxis WRITE setxAxis)

public:
    CustomPlotItem( QQuickItem* parent = 0 );
    virtual ~CustomPlotItem();
    // background properties
    QColor background() { return m_background; }
    void  setBackground(QColor value) { m_background = value; }
    CustomAxis* yAxis() const { return m_yAxis; }
    void  setyAxis(CustomAxis* value) { m_yAxis = value; }

    CustomAxis* xAxis() const { return m_xAxis; }
    void  setxAxis(CustomAxis* value) { m_xAxis = value; }

    void paint( QPainter* painter );

    Q_INVOKABLE void initCustomPlot();

protected:
    void routeMouseEvents( QMouseEvent* event );

    virtual void mousePressEvent( QMouseEvent* event );
    virtual void mouseReleaseEvent( QMouseEvent* event );
    virtual void mouseMoveEvent( QMouseEvent* event );
    virtual void mouseDoubleClickEvent( QMouseEvent* event );

    void setupQuadraticDemo( QCustomPlot* customPlot );

private:
    QCustomPlot* m_CustomPlot;
    QColor m_background;
    CustomAxis* m_xAxis;
    CustomAxis* m_yAxis;

private slots:
    void graphClicked( QCPAbstractPlottable* plottable );
    void onCustomReplot();
    void updateCustomPlotSize();

};
