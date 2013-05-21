#include "CDisplay.h"

CDisplay::CDisplay(QWidget *parent) :
    QWidget(parent),
    frequency1("106.500000"),
    frequency2("145.425000"),
    signal1(128),
    signal2(240),
    IF1(128),
    IF2(128),
    Filter1(230000),
    Filter2(15000),
    radio(0),
    Step1(0),
    Step2(0),
    Mode1("WFM"),
    Mode2("FM")
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
}

CDisplay::~CDisplay()
{

}

void CDisplay::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter) {
        if (radio == 0)
            frequency1 = format(frequency1,0);
        else frequency2 = format(frequency2,0);

        releaseKeyboard();

        repaint();
        if (radio == 0)
            emit frequencyChanged(frequency1.replace(".",""));
        else
            emit frequencyChanged(frequency2.replace(".",""));

        return;
    }
    if (radio == 0)
        frequency1.append( event->text());
    else frequency2.append( event->text());

    repaint();
}

void CDisplay::mousePressEvent(QMouseEvent *event)
{
    // Grab keyboard entry
    if (event->button() == Qt::LeftButton) {
        // Activate radio from rect
        if (rectFreq1.contains(event->pos())) {
            radio = 0;
            emit radioChanged(radio);
        }
        if (rectFreq2.contains(event->pos())) {
            radio = 1;
            emit radioChanged(radio);
        }
    }
    if (event->button() == Qt::RightButton) {
        //setIncrement(ts/10);
    }
    if (event->button() == Qt::MiddleButton) {
        grabKeyboard();
        if (radio == 0)
            frequency1 = "";
        else frequency2 = "";
    }
    repaint();
}

void CDisplay::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    if (event->delta() >0) {
        if (radio==0)
            frequency1 = format(frequency1,StepSize[Step1]);
        else frequency2 = format(frequency2,StepSize[Step2]);
    } else {
        if (radio==0)
            frequency1 = format(frequency1,StepSize[Step1] * -1);
        else frequency2 = format(frequency2,StepSize[Step2] * -1);
    }
    repaint();
    if (radio==0)
        emit frequencyChanged(frequency1.replace(".",""));
    else emit frequencyChanged(frequency2.replace(".",""));
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

    // Draw Filter
    drawFilter(&painter);

    // Draw Step
    drawStepSize(&painter);

    // draw Mode
    drawMode(&painter);
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
    rectFreq1 = r;
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

    // Radio activated ?
    if (radio == 0)
        p->setPen(QColor(Qt::blue));
    else
        p->setPen(QColor(Qt::gray));

    p->drawText(r,Qt::AlignCenter,frequency1);

    r = fm.boundingRect(frequency2);
    // Calculate width() to set on corect increment digit
    r.moveCenter(QPoint(size.width()*3/4,size.height()/3));
    rectFreq2 = r;
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

    // Radio activated ?
    if (radio == 1)
        p->setPen(QColor(Qt::blue));
    else
        p->setPen(QColor(Qt::gray));
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
    p->drawLine(QPointF((size.width()*0.25/grid),size.height()*1.33/3 + 17.0), QPointF((size.width()*0.25/grid)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()*0.25/grid)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal1)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()*0.25/grid)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()*0.25/grid)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()*0.25/grid)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
        }

    }

    p->setPen(pen);
    p->drawLine(QPointF((size.width()*2.25/grid),size.height()*1.33/3 + 17.0), QPointF((size.width()*2.25/grid)+252,size.height()*1.33/3 + 17.0));

    // Draw scale
    for (int i=0; i < 255; i+=5) {
        // square
        QRectF s(QPointF((size.width()*2.25/grid)+i,size.height()*1.33/3), QSizeF(2.0 +((i % 15) == 0 ?2.0:0.0), 11.0));
        if ( i < signal2)
            p->fillRect(s,Qt::blue);
        else
            p->fillRect(s,Qt::gray);

        // scale
        if ((i % 25) == 0 ) {
            QPen pendash(Qt::red, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            p->setPen(pendash);
            p->drawLine(QPointF((size.width()*2.25/grid)+i,size.height()*1.33/3 + 17.0), QPointF((size.width()*2.25/grid)+i,size.height()*1.66/3));
            p->setPen(Qt::white);
            p->drawText(QPointF((size.width()*2.25/grid)+i,size.height()*1.33/3 + 27.0), QString("%1").arg(i));
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
    QColor cyan(Qt::cyan);
    QBrush brush(cyan);
    p->setBrush(brush);

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

    // Selection
    pen.setWidth(1.0);
    p->setPen(pen);
    QFont font("Times", 8, QFont::Bold);
    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(QString("IF"));
    r.moveCenter(QPoint(axismid,axisy2 +13.0));
    p->setFont(font);
    p->drawRoundRect(QRectF(axismid - axiswidth/4.0, axisy2 +8.0, axiswidth * 2 / 4, 10.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("IF"));
    pen.setColor(QColor(Qt::cyan));
    p->setPen(pen);
    pen.setWidth(2.0);
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

    // Selection
    pen.setWidth(1.0);
    p->setPen(pen);
    r = fm.boundingRect(QString("IF"));
    r.moveCenter(QPoint(axismid,axisy2 +13.0));
    p->setFont(font);
    p->drawRoundRect(QRectF(axismid - axiswidth/4.0, axisy2 +8.0, axiswidth * 2 / 4, 10.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("IF"));

}

void CDisplay::drawFilter(QPainter *p)
{
    QRect size = this->geometry();
    int grid = 4;
    QPen penaxis(Qt::white, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    QPen pen(Qt::cyan, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    // Draw axis 1
    qreal axisx1 = (size.width()*1.75/grid);
    qreal axisx2 = (size.width()*1.25/grid);
    qreal axiswidth = axisx1 - axisx2;
    qreal axisy1 = size.height()*2/3;
    qreal axisy2 = size.height()*2.5/3 ;
    qreal axisheight = axisy2 -axisy1;
    qreal axismid = axisx1 + (axisx2 - axisx1)/2.0;
    qreal pos = 128*axiswidth/255.0;
    // Draw center axis
    p->setPen(penaxis);
    //draw bottom axis
    p->drawLine(QPointF(axisx1,axisy2), QPointF(axisx2,axisy2));
    penaxis.setStyle(Qt::DashLine);
    p->drawLine(QPointF(axismid, axisy1 ), QPointF(axismid,axisy2));
    penaxis.setStyle(Qt::SolidLine);

    // Draw Filter1 band
    p->setPen(pen);
    //top
    p->drawLine(QPointF(axisx2 + pos + (Filter1*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos - (Filter1*axiswidth/460000.0), axisy1 + axisheight * 2 /8));
    //left
    p->drawLine(QPointF(axisx2 + pos + (Filter1*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos + (Filter1*axiswidth/460000.0) + axiswidth/16.0 , axisy2));
    //right
    p->drawLine(QPointF(axisx2 + pos - (Filter1*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos - (Filter1*axiswidth/460000.0) - axiswidth/16.0 , axisy2));

    // Selection
    QColor cyan(Qt::cyan);
    QBrush brush(cyan);
    p->setBrush(brush);

    pen.setWidth(1.0);
    QFont font("Times", 8, QFont::Bold);
    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(QString("%1k").arg(Filter1/1000.0));
    r.moveCenter(QPoint(axismid,axisy2 +13.0));
    p->setFont(font);
    p->drawRoundRect(QRectF(axismid - axiswidth/4.0, axisy2 +8.0, axiswidth * 2 / 4, 10.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1k").arg(Filter1/1000));
    pen.setColor(QColor(Qt::cyan));
    pen.setWidth(2.0);
    p->setPen(pen);

    // Draw axis 2
    axisx1 = (size.width()*3.75/grid);
    axisx2 = (size.width()*3.25/grid);
    axismid = axisx1 + (axisx2 - axisx1)/2.0;
    pos = 128*axiswidth/255.0;
    p->setPen(penaxis);
    //draw bottom axis
    p->drawLine(QPointF(axisx1,axisy2), QPointF(axisx2,axisy2));
    penaxis.setStyle(Qt::DashLine);
    p->drawLine(QPointF(axismid, axisy1 ), QPointF(axismid,axisy2));
    penaxis.setStyle(Qt::SolidLine);

    // Draw Filter2 band
    p->setPen(pen);
    // top
    p->drawLine(QPointF(axisx2 + pos - (Filter2*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos +  (Filter2*axiswidth/460000.0), axisy1 + axisheight * 2 /8));
    //left
    p->drawLine(QPointF(axisx2 + pos - (Filter2*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos - (Filter2*axiswidth/460000.0) - axiswidth/16.0 , axisy2));
    //right
    p->drawLine(QPointF(axisx2 + pos + (Filter2*axiswidth/460000.0) , axisy1 + axisheight * 2 /8), QPointF(axisx2 + pos + (Filter2*axiswidth/460000.0) + axiswidth/16.0 , axisy2));

    // Selection
    pen.setWidth(1.0);
    p->setPen(pen);
    r = fm.boundingRect(QString("%1k").arg(Filter2/1000.0));
    r.moveCenter(QPoint(axismid,axisy2 +13.0));
    p->setFont(font);
    p->drawRoundRect(QRectF(axismid - axiswidth/4.0, axisy2 +8.0, axiswidth * 2 / 4, 10.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1k").arg(Filter2/1000));
    pen.setColor(QColor(Qt::cyan));

}

void CDisplay::drawStepSize(QPainter *p)
{
    QRect size = this->geometry();
    int left = size.width()/4;
    int right = size.width()*3/4;
    int top = size.height()*2/3;
    QColor cyan(Qt::cyan);
    QBrush brush(cyan);
    QFont font("Times", 8, QFont::Bold);
    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(QString("%1k").arg(StepSize[Step1]/1000.0));
    QPen pen(Qt::cyan, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    // First channel
    r.moveCenter(QPoint(left,top));
    p->setBrush(brush);
    p->setFont(font);
    p->drawRoundRect(QRectF(left - 20.0, top - 10.0, 40.0, 20.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1k").arg(StepSize[Step1]/1000.0));
    pen.setColor(QColor(Qt::cyan));

    r = fm.boundingRect(QString("%1k").arg(StepSize[Step2]/1000.0));
    r.moveCenter(QPoint(right,top));
    p->setBrush(brush);
    p->setFont(font);
    p->drawRoundRect(QRectF(right - 20.0, top - 10.0, 40.0, 20.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1k").arg(StepSize[Step2]/1000.0));
    pen.setColor(QColor(Qt::cyan));

}

void CDisplay::drawMode(QPainter *p)
{
    QRect size = this->geometry();
    int left = size.width()/4;
    int right = size.width()*3/4;
    int top = size.height()*2.3/3;
    QColor cyan(Qt::cyan);
    QBrush brush(cyan);
    QFont font("Times", 8, QFont::Bold);
    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(QString("%1").arg(Mode1));
    QPen pen(Qt::cyan, 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    r.moveCenter(QPoint(left,top));
    p->setBrush(brush);
    p->setFont(font);
    p->drawRoundRect(QRectF(left - 20.0, top - 10.0, 40.0, 20.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1").arg(Mode1));
    pen.setColor(QColor(Qt::cyan));

    r = fm.boundingRect(QString("%1").arg(Mode2));
    r.moveCenter(QPoint(right,top));
    p->setBrush(brush);
    p->setFont(font);
    p->drawRoundRect(QRectF(right - 20.0, top - 10.0, 40.0, 20.0));
    pen.setColor(QColor(Qt::black));
    p->setPen(pen);
    p->drawText(r, QString("%1k").arg(Mode2));
    pen.setColor(QColor(Qt::cyan));

}


QString CDisplay::format(QString frequency, int increment)
{
    // fill left side after dot with 0
    if (frequency.indexOf(".")>-1) {
        QString digit;
        QString unit;

        // First part of 106.5 eg 106 in unit
        unit = frequency.mid(0,frequency.indexOf("."));

        // Second part of 106.5 eg 5 in digit
        digit = frequency.mid(frequency.indexOf(".")+1).leftJustified(6,'0',true);
        // Limit unit to 3200
        if (unit.toInt()>3200) { unit = "3200"; }
        // the full frequency can be 3200.000000 do 10 digits
        frequency = unit +"."+digit;
    }

    // Add the increment
    frequency = frequency.replace(".",""); // remove Mhz point
    frequency = QString("%1").arg(frequency.toInt() + increment);
    frequency = frequency.insert(frequency.length()-6,"."); // reinsert Mhz point
    return frequency;
}

void CDisplay::setFrequency(QString value)
{
    if (radio == 0)
        frequency1 = value;
    else
        frequency2 = value;
    repaint();
}

void CDisplay::setFrequency(int value)
{
    QString frequency("%1");
    frequency = frequency.arg(value);
    frequency = frequency.insert(frequency.length()-6,".");
    if (radio == 0)
        frequency1 = frequency;
    else
        frequency2 = frequency;
    repaint();
}

void CDisplay::setSignal(int ch, int value)
{
    if (ch ==0)
        signal1 = value;
    else
        signal2 = value;
    repaint();
}

void CDisplay::setIF(int value)
{
    if (radio == 0)
        IF1 = value;
    else
        IF2 = value;
    repaint();
}

void CDisplay::setFilter(CCommand::filter filter)
{
    int value = 2800;
    switch(filter) {
        case CCommand::e28k : value=2800; break;
        case CCommand::e6k : value=6000; break;
        case CCommand::e15k : value=15000; break;
        case CCommand::e50k : value=50000; break;
        case CCommand::e230k : value=230000; break;
        default : value=230000;
    }
    if (radio == 0)
        Filter1 = value;
    else
        Filter2 = value;
    repaint();
}

void CDisplay::setRadio(int value)
{
    radio = value;
}

int CDisplay::getRadio()
{
    return radio;
}

void CDisplay::StepUp()
{
    if (radio==0) Step1++;
    else Step2++;
    repaint();
}

void CDisplay::StepDown()
{
    if (radio==0) Step1--;
    else Step2--;
    repaint();
}

void CDisplay::setMode(CCommand::mode mode)
{
    QString value("");
    switch(mode) {
        case CCommand::eWFM : value="WFM"; break;
        case CCommand::eFM : value="FM"; break;
        case CCommand::eAM : value="AM"; break;
        case CCommand::eCW : value="CW"; break;
        case CCommand::eUSB : value="USB"; break;
        case CCommand::eLSB : value="LSB"; break;
        default : value="WFM";
    }

    if (radio==0) Mode1=value;
    else Mode2=value;
    repaint();
}

void CDisplay::setStepFromValue(int value)
{
    // loop to find correct value
    int pos = 0;
    for (int i=0; i< MAX_STEP; i++) {
        if (StepSize[i] == value) {
            pos = i;
            break;
        }
    }
    if (radio==0) Step1=pos;
    else Step2=pos;

}

int CDisplay::getStep(int radionum)
{
    if (radionum==0) return StepSize[Step1];
    else return StepSize[Step2];
}
