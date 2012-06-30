#include "CStatusWidget.h"
#include <QDebug>
#include <qpainter.h>

CStatusWidget::CStatusWidget(QWidget *parent) :
    QWidget(parent)
,state(false)
{
    layout = new QHBoxLayout(this);
    led.setFixedSize(10,10);
    layout->addWidget(&status);
    layout->addWidget(&led);
}

void CStatusWidget::slotUpdate(QString value)
{
    status.setText(value);
}

void CStatusWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.fillRect(led.pos().x(), led.pos().y(), 10, 10, (state == true) ? Qt::green : Qt::red);
    qDebug() << "State " << state;
}

void CStatusWidget::setState(bool value)
{
    state = value;
}
