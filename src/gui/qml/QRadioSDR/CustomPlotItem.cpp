#include "CustomPlotItem.h"
#include "qcustomplot.h"
#include <QDebug>

CustomPlotItem::CustomPlotItem( QQuickItem* parent ) : QQuickPaintedItem( parent )
    , m_CustomPlot( nullptr )
{
    setFlag( QQuickItem::ItemHasContents, true );
    // setRenderTarget(QQuickPaintedItem::FramebufferObject);
    // setAcceptHoverEvents(true);
    setAcceptedMouseButtons( Qt::AllButtons );

    connect( this, &QQuickPaintedItem::widthChanged, this, &CustomPlotItem::updateCustomPlotSize );
    connect( this, &QQuickPaintedItem::heightChanged, this, &CustomPlotItem::updateCustomPlotSize );
}

CustomPlotItem::~CustomPlotItem()
{
    delete m_CustomPlot;
    m_CustomPlot = nullptr;
}

void CustomPlotItem::initCustomPlot()
{
    m_CustomPlot = new QCustomPlot();

    updateCustomPlotSize();
    // Assign background color
    m_CustomPlot->setBackground(QBrush(m_background));
    // X axis
    m_CustomPlot->xAxis->setLabel(m_xAxis->label());
    m_CustomPlot->xAxis->setLabelColor(m_xAxis->color());
    m_CustomPlot->xAxis->setBasePen(QPen(m_xAxis->color()));
    m_CustomPlot->xAxis->setTickPen(QPen(m_xAxis->color()));
    m_CustomPlot->xAxis->setSubTickPen(QPen(m_xAxis->color()));
    m_CustomPlot->xAxis->setTickLabelColor(m_xAxis->color());
    // Y axis
    m_CustomPlot->yAxis->setLabel(m_yAxis->label());
    m_CustomPlot->yAxis->setLabelColor(m_yAxis->color());
    m_CustomPlot->yAxis->setBasePen(QPen(m_yAxis->color()));
    m_CustomPlot->yAxis->setTickPen(QPen(m_yAxis->color()));
    m_CustomPlot->yAxis->setSubTickPen(QPen(m_yAxis->color()));
    m_CustomPlot->yAxis->setTickLabelColor(m_yAxis->color());

    // set axes ranges, so we see all data:
    m_CustomPlot->xAxis->setRange( m_xAxis->min(), m_xAxis->max() );
    m_CustomPlot->yAxis->setRange( m_yAxis->min(), m_yAxis->max() );

    setupQuadraticDemo( m_CustomPlot );

    connect( m_CustomPlot, &QCustomPlot::afterReplot, this, &CustomPlotItem::onCustomReplot );

    m_CustomPlot->replot();
}


void CustomPlotItem::paint( QPainter* painter )
{
    if (m_CustomPlot)
    {
        QPixmap    picture( boundingRect().size().toSize() );
        QCPPainter qcpPainter( &picture );

        //m_CustomPlot->replot();
        m_CustomPlot->toPainter( &qcpPainter );

        painter->drawPixmap( QPoint(), picture );
    }
}

void CustomPlotItem::mousePressEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::mouseReleaseEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::mouseMoveEvent( QMouseEvent* event )
{
    routeMouseEvents( event );
}

void CustomPlotItem::mouseDoubleClickEvent( QMouseEvent* event )
{
    qDebug() << Q_FUNC_INFO;
    routeMouseEvents( event );
}

void CustomPlotItem::graphClicked( QCPAbstractPlottable* plottable )
{
    qDebug() << Q_FUNC_INFO << QString( "Clicked on graph '%1 " ).arg( plottable->name() );
}

void CustomPlotItem::routeMouseEvents( QMouseEvent* event )
{
    if (m_CustomPlot)
    {
        QMouseEvent* newEvent = new QMouseEvent( event->type(), event->localPos(), event->button(), event->buttons(), event->modifiers() );
        //QCoreApplication::sendEvent( m_CustomPlot, newEvent );
        QCoreApplication::postEvent( m_CustomPlot, newEvent );
    }
}

void CustomPlotItem::updateCustomPlotSize()
{
    if (m_CustomPlot)
    {
        m_CustomPlot->setGeometry( 0, 0, width(), height() );
    }
}

void CustomPlotItem::onCustomReplot()
{
    qDebug() << Q_FUNC_INFO;
    update();
}

void CustomPlotItem::setupQuadraticDemo( QCustomPlot* customPlot )
{
    // generate some data:
    QVector<double> lx( 101 ), ly( 101 ); // initialize with entries 0..100
    for (int i = 0; i < 101; ++i)
    {
        lx[i] = i+106.9;             //
        ly[i] = lx[i] - 106.0;                    // linear
    }
    // create graph and assign data to it:
    customPlot->addGraph();
    customPlot->graph( 0 )->setPen( QPen( Qt::red ) );
    customPlot->graph( 0 )->setSelectedPen( QPen( Qt::blue, 2 ) );
    customPlot->graph( 0 )->setData( lx, ly );

    customPlot ->setInteractions( QCP::iRangeZoom | QCP::iSelectPlottables );
    connect( customPlot, SIGNAL( plottableClick( QCPAbstractPlottable*, QMouseEvent* ) ), this, SLOT( graphClicked( QCPAbstractPlottable* ) ) );
}
