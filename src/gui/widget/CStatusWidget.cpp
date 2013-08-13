/**********************************************************************************************
  Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
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
    p.fillRect(led.pos().x(), led.pos().y(), 10, 10, (state == true) ? QColor(0,255,0,255) : QColor(255,0,0,255));
}

void CStatusWidget::setState(bool value)
{
    state = value;
}
