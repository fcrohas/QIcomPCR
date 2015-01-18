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
#ifndef CFIR_H
#define CFIR_H

#include <QObject>
#include <math.h>
#include <stdint.h>
#include <limits>
#include <QDebug>

template<class T>
class CFIR
{
public:
    explicit CFIR();
    ~CFIR();
    void setWindow(double *win);
    void lowpass(double frequency);
    void highpass(double frequency);
    void bandpass(double centerfreq, double bandwidth);
    void stopband(double centerfreq, double bandwidth);
    void setOrder(int value);
    void setSampleRate(double value);
    void apply(T *&in, int size);
    //void apply(int16_t *in, int size);
    void intToFloat( int16_t *input, double *output, int length );
    void floatToInt( double *input, int16_t *output, int length );
    void convert();
private:
    double *win;
    double *fir;
    T *tfir;
    T *buffer;
    double *bufferd;
    int N;
    double M;
    double wc;
    double fs;
    T tmin;
    T tmax;
    double scaleFactor;
    bool update;
};

#endif // CFIR_H
