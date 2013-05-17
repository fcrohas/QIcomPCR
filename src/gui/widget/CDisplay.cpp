#include "CDisplay.h"

CDisplay::CDisplay(QWidget *parent) :
    QWidget(parent),
    frequency1("0.000.000.000"),
    frequency2("0.000.000.000"),
    signal1(128),
    signal2(240),
    IF1(128),
    IF2(128)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

CDisplay::~CDisplay()
{

}

void CDisplay::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // Draw background
    painter.fillRect(painter.window(),Qt::black);

    // Draw frequency
    drawFrequency(&painter);

    // Draw signal
    drawSignal(&painter);

    // Draw IF
    drawIF(&painter);
}

void CDisplay::drawFrequency(QPainter *p)
{
    // Now draw frequency
    QRect size = this->geometry();
    QFont font("Times", 28, QFont::Normal);;

    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(frequency1);
    // Calculate width() to set on corect increment digit
    r.moveCenter(QPoint(size.width()/4,size.height()/3));
    p->setPen(Qt::white);
    p->setFont(font);

    p->drawText(r.adjusted(-1,-1,-1,-1),Qt::AlignCenter,frequency1);
    p->drawText(r.adjusted( 0,-1, 0,-1),Qt::AlignCenter,frequency1);
    p->drawText(r.adjusted(+1,-1,+1,-1),Qt::AlignCenter,frequency1);

    p->drawText(r.adjusted(-1, 0,-1, 0),Qt::AlignCenter,frequency1);
    p->drawText(r.adjusted(+1, 0,+1, 0),Qt::AlignCenter,frequency1);

    p->drawText(r.adjusted(-1,+1,-1,+1),Qt::AlignCenter,frequency1);
    p->drawText(r.adjusted( 0,+1, 0,+1),Qt::AlignCenter,frequency1);
    p->drawText(r.adjusted(+1,+1,+1,+1),Qt::AlignCenter,frequency1);

    p->setPen(QColor(Qt::cyan));
    p->drawText(r,Qt::AlignCenter,frequency1);

    r = fm.boundingRect(frequency2);
    // Calculate width() to set on corect increment digit
    r.moveCenter(QPoint(size.width()*3/4,size.height()/3));
    p->setPen(Qt::white);
    p->setFont(font);

    p->drawText(r.adjusted(-1,-1,-1,-1),Qt::AlignCenter,frequency2);
    p->drawText(r.adjusted( 0,-1, 0,-1),Qt::AlignCenter,frequency2);
    p->drawText(r.adjusted(+1,-1,+1,-1),Qt::AlignCenter,frequency2);

    p->drawText(r.adjusted(-1, 0,-1, 0),Qt::AlignCenter,frequency2);
    p->drawText(r.adjusted(+1, 0,+1, 0),Qt::AlignCenter,frequency2);

    p->drawText(r.adjusted(-1,+1,-1,+1),Qt::AlignCenter,frequency2);
    p->drawText(r.adjusted( 0,+1, 0,+1),Qt::AlignCenter,frequency2);
    p->drawText(r.adjusted(+1,+1,+1,+1),Qt::AlignCenter,frequency2);

    p->setPen(QColor(Qt::cyan));
    p->drawText(r,Qt::AlignCenter,frequency2);

}

void CDisplay::drawSignal(QPainter *p)
{
    QRect size = this->geometry();
    int grid = 4;
    QFont font("Times", 8, QFont::Normal);;
    QFontMetrics    fm(font);
    p->setFont(font);

    // Draw horizontal line
    QPen pen(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->drawLine(QPointF((size.width()*0.5/grid),size.height()*1.33/3 + 17.0), QPointF((size.width()*0.5/grid)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()*0.5/grid)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal1)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()*0.5/grid)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()*0.5/grid)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()*0.5/grid)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
        }

    }

    p->setPen(pen);
    p->drawLine(QPointF((size.width()*2.5/grid),size.height()*1.33/3 + 17.0), QPointF((size.width()*2.5/grid)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()*2.5/grid)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal2)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()*2.5/grid)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()*2.5/grid)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()*2.5/grid)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
        }

    }

}

void CDisplay::drawIF(QPainter *p)
{
    QRect size = this->geometry();
    int grid = 4;
    int filterwidth = 15.0;
    QPen penaxis(Qt::white, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen(Qt::cyan, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    // Draw axis 1
    qreal axisx1 = (size.width()*0.75/grid);
    qreal axisx2 = (size.width()*0.25/grid);
    qreal axiswidth = axisx1 - axisx2;
    qreal axisy1 = size.height()*2/3;
    qreal axisy2 = size.height()*2.5/3 ;
    qreal axisheight = axisy2 -axisy1;
    qreal axismid = axisx1 + (axisx2 - axisx1)/2.0;
    qreal pos = IF1*axiswidth/255.0;
    // Draw center axis
    p->setPen(penaxis);
    //draw bottom axis
    p->drawLine(QPointF(axisx1,axisy2), QPointF(axisx2,axisy2));
    penaxis.setStyle(Qt::DashLine);
    p->drawLine(QPointF(axismid, axisy1 ), QPointF(axismid,axisy2));
    penaxis.setStyle(Qt::SolidLine);

    // Draw IF1 band
    p->setPen(pen);
    //top
    p->drawLine(QPointF(axisx2 + pos + (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos -  (filterwidth*axiswidth/255.0), axisy1 + axisheight * 2 /8));
    //left
    p->drawLine(QPointF(axisx2 + pos + (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos + axiswidth/8.0 , axisy2));
    //right
    p->drawLine(QPointF(axisx2 + pos - (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos - axiswidth/8.0 , axisy2));

    // Draw axis 2
    axisx1 = (size.width()*2.75/grid);
    axisx2 = (size.width()*2.25/grid);
    axismid = axisx1 + (axisx2 - axisx1)/2.0;
    pos = IF2*axiswidth/255.0;
    p->setPen(penaxis);
    //draw bottom axis
    p->drawLine(QPointF(axisx1,axisy2), QPointF(axisx2,axisy2));
    penaxis.setStyle(Qt::DashLine);
    p->drawLine(QPointF(axismid, axisy1 ), QPointF(axismid,axisy2));
    penaxis.setStyle(Qt::SolidLine);

    // Draw IF1 band
    p->setPen(pen);
    // top
    p->drawLine(QPointF(axisx2 + pos - (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos +  (filterwidth*axiswidth/255.0), axisy1 + axisheight * 2 /8));
    //left
    p->drawLine(QPointF(axisx2 + pos - (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos - axiswidth/8.0 , axisy2));
    //right
    p->drawLine(QPointF(axisx2 + pos + (filterwidth*axiswidth/255.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos + axiswidth/8.0 , axisy2));

}

void CDisplay::setFrequency1(QString value)
{
    frequency1 = value;
}

void CDisplay::setFrequency2(QString value)
{
    frequency1 = value;
}

void CDisplay::setSignal1(int value)
{
    signal1 = value;
    repaint();
}

void CDisplay::setSignal2(int value)
{
    signal2 = value;
    repaint();
}

void CDisplay::setIF1(int value)
{
    IF1 = value;
    repaint();
}

void CDisplay::setIF2(int value)
{
    IF2 = value;
    repaint();
}
