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
#ifndef CLCDWIDGET_H
#define CLCDWIDGET_H

#include <QWidget>
#include <QHBoxLayout>

class CLcdWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CLcdWidget(QWidget *parent = 0);
    void setFrequency(QString value);
    void setIncrement(int value);
    void drawFrequency();
    
signals:
    void frequencyChanged(QString&);
    
public slots:

private:
    QHBoxLayout *layout;
    QString frequency;
    bool keyEnterPressed;
    // ts
    int ts;
    // Format frequency for human readable
    QString format(QString &value, int increment);

protected:
    void paintEvent(QPaintEvent *event);
    void keyPressEvent ( QKeyEvent * event );
    void mousePressEvent (QMouseEvent * event);
    void wheelEvent(QWheelEvent *event);
};


#endif // CLCDWIDGET_H
