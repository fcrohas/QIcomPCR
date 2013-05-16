#include "CDisplay.h"

CDisplay::CDisplay(QWidget *parent) :
    QWidget(parent),
    frequency1("0.000.000.000"),
    frequency2("0.000.000.000"),
    signal1(128),
    signal2(240)
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

    QFont font("Times", 8, QFont::Normal);;
    QFontMetrics    fm(font);
    p->setFont(font);

    // Draw horizontal line
    QPen pen(Qt::gray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    p->setPen(pen);
    p->drawLine(QPointF((size.width()/12),size.height()*1.33/3 + 17.0), QPointF((size.width()/12)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()/12)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal1)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()/12)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()/12)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()/12)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
        }

    }

    p->setPen(pen);
    p->drawLine(QPointF((size.width()*6/12),size.height()*1.33/3 + 17.0), QPointF((size.width()*6/12)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()*6/12)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal2)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()*6/12)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()*6/12)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()*6/12)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
        }

    }

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
