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
#ifndef CDISPLAY_H
#define CDISPLAY_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QWheelEvent>
#include <CCommand.h>

#define MAX_STEP 18
const int StepSize[MAX_STEP] = {1,10,50,100,500,1000,2500,6000,8333,10000,12500,15000,20000,25000,50000,100000,500000,1000000 };

class CDisplay : public QWidget
{
    Q_OBJECT
public:
    explicit CDisplay(QWidget *parent = 0);
    ~CDisplay();

    // Frequency
    void setFrequency(QString value);
    void setFrequency(int value);

    void setSignal(int ch, int value);

    void setIF(int value);

    void setFilter(CCommand::filter filter);

    void setStepFromValue(int value);
    int  getStep(int radionum);

    void setRadio(int value);
    int  getRadio();

    void StepUp();
    void StepDown();

    void setMode(CCommand::mode mode);

    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);

    void wheelEvent(QWheelEvent *event);
    
signals:
    void frequencyChanged(QString value);
    void radioChanged(int radio);
    
public slots:

private:

    // Frequency
    QString frequency1;
    QString frequency2;
    QRect rectFreq1;
    QRect rectFreq2;

    // Signal
    int signal1;
    int signal2;

    // IF
    int IF1;
    int IF2;

    // Filter
    int Filter1;
    int Filter2;

    // Step
    int Step1;
    int Step2;

    // Radio
    int radio;

    // Mode
    QString Mode1;
    QString Mode2;

    // Frequency
    void drawFrequency(QPainter *p);

    // power
    void drawSignal(QPainter *p);

    // IF
    void drawIF(QPainter *p);

    // Filter
    void drawFilter(QPainter *p);

    // Step Size
    void drawStepSize(QPainter *p);

    // Modulation
    void drawMode(QPainter *p);

    // Format frequency
    QString format(QString frequency, int increment);
};

#endif // CDISPLAY_H
