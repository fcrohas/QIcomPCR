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
#ifndef CWINDOWFUNC_H
#define CWINDOWFUNC_H

#include <QObject>
#include <math.h>
#include <qdebug.h>

class CWindowFunc : public QObject
{
    Q_OBJECT
public:
    explicit CWindowFunc(QObject *parent = 0);
    void hann();
    void hamming();
    void blackman();
    void blackmanharris();
    void rectangle();
    int kaiser(double attenuation, double frequency, double width, int samplerate);
    void init(int size);
    double get(int i);
    double * getWindow();

signals:
    
public slots:

private:
    double *win;
    int N;
    int factorial(int n);
    double bessel(double x);
};

#endif // CWINDOWFUNC_H
