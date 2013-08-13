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
#ifndef CIIR_H
#define CIIR_H

#include <QObject>
#include <QDebug>
#include <math.h>

class CIIR : public QObject
{
    Q_OBJECT
public:
    explicit CIIR(QObject *parent = 0);
    ~CIIR();
    void setWindow(double *win);
    void lowpass(double frequency);
    void highpass(double frequency);
    void bandpass(double centerfreq, double bandwidth);
    void stopband(double centerfreq, double bandwidth);
    void setOrder(int value);
    void setSampleRate(double value);
    void apply(double *&in, int size);

signals:
    
public slots:

private:
    double *win;
    double *iirA;
    double *iirB;
    double *bufferin;
    double *bufferout;
    double wc;
    double fs;
    // Filter order only even to calculate passes
    int N;
    // Second order coeff lowpass
    double a0l,a1l,a2l;
    double b1l,b2l;
    // Second order coeff highpass
    double a0h,a1h,a2h;
    double b1h,b2h;
    // High or low pass or both
    bool high;
    bool low;
    bool band;
    bool stop;
    bool update;
};

#endif // CIIR_H
