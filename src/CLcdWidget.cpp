#include "CLcdWidget.h"
#include <QPainter>
#include <QKeyEvent>
#include <QDebug>

CLcdWidget::CLcdWidget(QWidget *parent) :
    QWidget(parent)
  ,frequency("106.500000")
  ,keyEnterPressed(true)
  ,ts(1000)
{
    layout = new QHBoxLayout(this);
    setFixedSize(300,50);
}

void CLcdWidget::setFrequency(QString value)
{
    frequency = value;
    repaint();
}

void CLcdWidget::mousePressEvent(QMouseEvent *event)
{
    // Grab keyboard entry
    if (event->button() == 1) {
        setIncrement(ts*10);
    }
    if (event->button() == 2) {
        setIncrement(ts/10);
    }
    if (event->button() == 4) {
        grabKeyboard();
        frequency = "";
        // Initialize Enter sequence
        keyEnterPressed = false;
    }
    repaint();
}

void CLcdWidget::setIncrement(int value)
{
    ts = value;
}

void CLcdWidget::wheelEvent(QWheelEvent *event)
{
    int numDegrees = event->delta() / 8;
    int numSteps = numDegrees / 15;
    if (event->delta() >0) {
        frequency = format(frequency,ts);
    } else {
        frequency = format(frequency,ts * -1);
    }
    repaint();
    emit frequencyChanged(frequency.replace(".",""));
}

void CLcdWidget::drawFrequency()
{

}

QString CLcdWidget::format(QString &value, int increment)
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

void CLcdWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Enter) {
        frequency = format(frequency,0);

        keyEnterPressed = true;
        releaseKeyboard();

        repaint();

        emit frequencyChanged(frequency.replace(".",""));

        return;
    }

    frequency.append( event->text());

    repaint();
}

void CLcdWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QRect size = layout->geometry();

    // Create black rect
    p.fillRect(size, QColor(Qt::black));

    // Inside it create a gray rect for background
    p.fillRect(size.adjusted(5,5,-5,-5), QColor(Qt::gray));

    // Now draw frequency
    QFont font("Times", 28, QFont::Bold);;

    QFontMetrics    fm(font);
    QRect           r = fm.boundingRect(frequency);
    // Calculate width() to set on corect increment digit
    r.moveCenter(QPoint(size.width()/2,size.height()/2));
    p.setPen(Qt::white);
    p.setFont(font);

    p.drawText(r.adjusted(-1,-1,-1,-1),Qt::AlignCenter,frequency);
    p.drawText(r.adjusted( 0,-1, 0,-1),Qt::AlignCenter,frequency);
    p.drawText(r.adjusted(+1,-1,+1,-1),Qt::AlignCenter,frequency);

    p.drawText(r.adjusted(-1, 0,-1, 0),Qt::AlignCenter,frequency);
    p.drawText(r.adjusted(+1, 0,+1, 0),Qt::AlignCenter,frequency);

    p.drawText(r.adjusted(-1,+1,-1,+1),Qt::AlignCenter,frequency);
    p.drawText(r.adjusted( 0,+1, 0,+1),Qt::AlignCenter,frequency);
    p.drawText(r.adjusted(+1,+1,+1,+1),Qt::AlignCenter,frequency);

    p.setPen(QColor(Qt::black));
    p.drawText(r,Qt::AlignCenter,frequency);

    p.setBrush(Qt::Dense4Pattern);
    QColor lcdColor(Qt::gray);
    lcdColor.setAlpha(80);
    p.fillRect(size.adjusted(5,5,-5,-5),lcdColor);

}


