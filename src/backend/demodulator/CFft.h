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
#ifndef CFFT_H
#define CFFT_H

#include <QObject>
#include <filter/CWindowFunc.h>
#include <math.h>
#include <stdint.h>
#include <QDebug>
#include <consts.h>
#include <fftw3.h>


class CFFT : public QObject
{
    Q_OBJECT
public:
    explicit CFFT(QObject *parent = 0, int size=512);
    ~CFFT();
    void decode(int16_t *buffer, int size, double *xval, double *yval);
    void initBuffer(int size);
    void initFFT(int size);
    // Available Window function for FFT
    enum windowFunction {Rectangle=1, Blackman=2, BlackmanHarris=3, Hann=4, Hamming=5};
    void setWindow(windowFunction fct, int size);

signals:
    void sigRawSamples(double *xval, double *yval, int size);
    
public slots:

private:
     CWindowFunc *winfunc;
     double *xval;
     double *yval;
     double *window;
     fftw_complex *in1, *in2;
     fftw_complex *out1, *out2;
     fftw_plan ch1, ch2;
     int bufferBlock;
     int fftsize;
     int channelSize;
     bool update;
};

#endif // CFFT_H
