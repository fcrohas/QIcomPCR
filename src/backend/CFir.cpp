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
#include "CFir.h"

CFIR::CFIR(QObject *parent) :
    QObject(parent),
    win(NULL),
    fir(NULL),
    buffer(NULL),
    update(false)
{

}

CFIR::~CFIR()
{
    delete [] buffer;
    buffer = NULL;
}

void CFIR::setWindow(double *value)
{
    win = value;
}

void CFIR::lowpass(double frequency)
{
    update = true;
    QString dump("");
    wc = (2.0 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M)
            fir[i] = wc/M_PI;
        else
            fir[i] = sin(wc*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
        dump.append(QString("%1;").arg(fir[i]));
    }
    qDebug() << "Hd=["<< dump << "]";
    delete [] buffer;
    buffer = NULL;
    update = false;
}

void CFIR::highpass(double frequency)
{
    update = true;
    QString dump("");
    wc = (2.0 * M_PI * frequency) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M)
            fir[i] = - sin(wc*(i-M))/(M_PI*(i-M));
        else
            fir[i] = 1 - wc / M_PI;
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
        dump.append(QString("%1;").arg(fir[i]));
    }
    qDebug() << "Hd=["<< dump << "]";
    delete [] buffer;
    buffer = NULL;
    update = false;
}

void CFIR::bandpass(double centerfreq, double bandwidth)
{
    update = true;
    QString dump("");
    double wc1 = (2.0 * M_PI * (centerfreq-bandwidth/2.0)) / fs;
    double wc2 = (2.0 * M_PI * (centerfreq+bandwidth/2.0)) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M)
            fir[i] = (wc2 - wc1)/M_PI;
        else
            fir[i] = sin(wc2*(i-M))/(M_PI*(i-M)) - sin(wc1*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
        dump.append(QString("%1;").arg(fir[i]));
    }
    qDebug() << "Hd=["<< dump << "]";
    delete [] buffer;
    buffer = NULL;
    update = false;
}

void CFIR::stopband(double centerfreq, double bandwidth)
{
    update = true;
    QString dump("");
    double wc1 = (2.0 * M_PI * (centerfreq-bandwidth/2.0)) / fs;
    double wc2 = (2.0 * M_PI * (centerfreq+bandwidth/2.0)) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M)
            fir[i] = 1.0 - (wc2 - wc1)/M_PI;
        else
            fir[i] = sin(wc1*(i-M))/(M_PI*(i-M)) - sin(wc2*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
        dump.append(QString("%1;").arg(fir[i]));
    }
    qDebug() << "Hd=["<< dump << "]";
    delete [] buffer;
    buffer = NULL;
    update = false;
}

void CFIR::setOrder(int value)
{
    N = value+1;
    M = value / 2.0;
    if (fir == NULL) {
        fir = new double[N];
    }
    else {
        delete [] fir;
        fir = new double[N];
    }
}

void CFIR::setSampleRate(double value)
{
    fs = value;
}

void CFIR::apply(double *&in, int size)
{
    if (update)
        return;
    if (buffer == NULL) {
        buffer = new double[size];
    }
    // loop on buffer and apply filter
#if 1
    for (int i=0; i<size; i++) {
        double sample = 0.0;
        // act as a ring buffer for current and previous call
        buffer[i] = in[i];
        // direct fir filter
        for (int j=0; j<N; j++) {
            if ((i-j) < 0)
                sample += buffer[size+i-j] * fir[j]; // use previous buffer call values
            else
                sample += buffer[i-j] * fir[j]; // use current buffer values
        }
        // Save
        in[i] = sample;
    }
#else
    __m128d X, Y, Z;
    for (int i=0; i<size; i++) {
        double sample = 0.0;
        // act as a ring buffer for current and previous call
        buffer[i] = in[i];
        // direct fir filter
        for (int j=0; j<N; j+=4) {
            if ((i-j) < 0) {
                X = _mm_load_pd(&buffer[size+i-j]);
            }
            else {
                X = _mm_load_pd(&buffer[i-j]);
            }
            Y = _mm_load_pd(&fir[j]);
            X = _mm_mul_pd(X, Y);
            Z = _mm_setzero_pd();
            Z = _mm_add_pd(X, Z);
        }
        // Save
        for(int j=0; j<4; j++) {
            sample = Z.m128d_f64[j] + sample;
        }
        in[i] = sample;
    }
#endif
}
