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

template<class T>
CFIR<T>::CFIR() :
    win(NULL),
    fir(NULL),
    buffer(NULL),
    update(false),
    scaleFactor(1.0)
{
    qDebug() << "CFir Contructor\r\n";
    tmin = std::numeric_limits<T>::min(); // minimum value
    //if (tmin < -127) tmin = -1.0;
    tmax = std::numeric_limits<T>::max(); // maximum value
    //if (tmax > 127) tmax = 1.0;
}

template<class T>
CFIR<T>::~CFIR()
{
    delete [] buffer;
    buffer = NULL;
}

template<class T>
void CFIR<T>::setWindow(double *value)
{
    win = value;
}

template<class T>
void CFIR<T>::lowpass(double frequency)
{
    update = true;
    wc = (2.0 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M) {
            fir[i] = wc/M_PI;
            firQ[i] = wc/M_PI;
        }
        else {
            fir[i] = sin(wc*(i-M))/(M_PI*(i-M));
            firQ[i] = cos(wc*(i-M))/(M_PI*(i-M));
        }
        // Apply window
        if (win != NULL) {
            fir[i] = fir[i] * win [i];
            firQ[i] = firQ[i] * win [i];
        }
    }
    convert();
    delete [] buffer;
    buffer = NULL;
    update = false;
}

template<class T>
void CFIR<T>::highpass(double frequency)
{
    update = true;
    wc = (2.0 * M_PI * frequency) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M) {
            fir[i] = - sin(wc*(i-M))/(M_PI*(i-M));
            firQ[i] = - cos(wc*(i-M))/(M_PI*(i-M));
        }
        else {
            fir[i] = 1 - wc / M_PI;
            firQ[i] = 1 - wc / M_PI;
        }
        // Apply window
        if (win != NULL) {
            fir[i] = fir[i] * win [i];
            firQ[i] = firQ[i] * win [i];
        }
    }
    convert();
    delete [] buffer;
    buffer = NULL;
    update = false;
}

template<class T>
void CFIR<T>::bandpass(double centerfreq, double bandwidth)
{
    update = true;
    double wc1 = (2.0 * M_PI * (centerfreq-bandwidth/2.0)) / fs;
    double wc2 = (2.0 * M_PI * (centerfreq+bandwidth/2.0)) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M) {
            fir[i] = (wc2 - wc1)/M_PI;
            firQ[i] = (wc2 - wc1)/M_PI;
        }
        else {
            fir[i] = sin(wc2*(i-M))/(M_PI*(i-M)) - sin(wc1*(i-M))/(M_PI*(i-M));
            firQ[i] = cos(wc2*(i-M))/(M_PI*(i-M)) - cos(wc1*(i-M))/(M_PI*(i-M));
        }
        // Apply window
        if (win != NULL) {
            fir[i] = fir[i] * win [i];
            firQ[i] = firQ[i] * win [i];
        }
    }
    convert();
    delete [] buffer;
    buffer = NULL;
    update = false;
}

template<class T>
void CFIR<T>::stopband(double centerfreq, double bandwidth)
{
    update = true;
    double wc1 = (2.0 * M_PI * (centerfreq-bandwidth/2.0)) / fs;
    double wc2 = (2.0 * M_PI * (centerfreq+bandwidth/2.0)) / fs;
    //wc = (2 * M_PI * frequency) / fs;
    for (int i=0; i<N; i++) {
        if (i==M) {
            fir[i] = 1.0 - (wc2 - wc1)/M_PI;
            firQ[i] = 1.0 - (wc2 - wc1)/M_PI;
        }
        else {
            fir[i] = sin(wc1*(i-M))/(M_PI*(i-M)) - sin(wc2*(i-M))/(M_PI*(i-M));
            firQ[i] = cos(wc1*(i-M))/(M_PI*(i-M)) - cos(wc2*(i-M))/(M_PI*(i-M));
        }
        // Apply window
        if (win != NULL) {
            fir[i] = fir[i] * win [i];
            firQ[i] = firQ[i] * win [i];
        }
    }
    convert();
    delete [] buffer;
    buffer = NULL;
    update = false;
}

template<class T>
void CFIR<T>::setOrder(int value)
{
    N = value+1;
    M = value / 2.0;
    if (fir == NULL) {
        tfir = new T[N];
        fir = new double[N];
        firQ = new double[N];
    }
    else {
        delete [] firQ;
        delete [] fir;
        delete [] tfir;
        tfir = new T[N];
        fir = new double[N];
        firQ = new double[N];
    }
}

template<class T>
void CFIR<T>::setSampleRate(double value)
{
    fs = value;
}


// the FIR filter function
template<class T>
void CFIR<T>::apply(T *&in, int size)
{
    if (update)
        return;
    if (in == NULL) {
        return;
    }

    if (buffer == NULL) {
        buffer = new T[size*2];
    }
    // loop on buffer and apply filter
    T *inputp;
    T acc;
    // put the new samples at the high end of the buffer
    memcpy( &buffer[N - 1], in,
            size * sizeof(T) );

    // apply the filter to each input sample
    for (int n = 0; n < size; n++ ) {
        // calculate output n
        inputp = &buffer[N - 1 + n];
        acc = 0;
        for (int k = 0; k < N; k++ ) {
            acc += fir[k] * (*inputp--);
        }
        in[n] = acc;
    }
    // shift input samples back in time for next time
    memmove( &buffer[0], &buffer[size],
            (N - 1) * sizeof(T) );
}

// the FIR filter function
template<class T>
void CFIR<T>::applyIQ(T *&in, int size)
{
    if (update)
        return;
    if (in == NULL) {
        return;
    }

    if (buffer == NULL) {
        buffer = new T[size*2];
    }
    // loop on buffer and apply filter
    T *inputpI;
    T *inputpQ;
    T accI;
    T accQ;
    // put the new samples at the high end of the buffer
    memcpy( &buffer[N - 1], in,
            size * sizeof(T) );

    // apply the filter to each input sample
    for (int n = 0; n < size; n++ ) {
        // calculate output n
        inputpI = &buffer[N - 1 + n]; // I sample
        inputpQ = &buffer[N - 1 + (n+1)]; // Q sample
        accI = 0;
        accQ = 0;
        for (int k = 0; k < N; k++ ) {
            accI += fir[k] * (*inputpI--);
            accQ += firQ[k] * (*inputpQ--);
        }
        in[n] = accI;
        in[n+1] = accQ;
    }
    // shift input samples back in time for next time
    memmove( &buffer[0], &buffer[size],
            (N - 1) * sizeof(T) );
}

template<class T>
void CFIR<T>::convert() {
    double minvalue = 0.0,maxvalue=0.0;
    // check for min/max value
    for (int i=0; i < N; i++) {
        if (fir[i]<minvalue) minvalue=fir[i];
        if (fir[i]>maxvalue) maxvalue=fir[i];
    }
    scaleFactor = std::min(abs(tmin/minvalue), abs(tmax/maxvalue));
    if (scaleFactor > 16384) scaleFactor = 16384;
    //qDebug() << "scale factor is " << scaleFactor;
    if (scaleFactor == 0) scaleFactor = 1.0;
    for (int i=0; i < N; i++) {
        tfir[i] = fir[i] * scaleFactor;
    }
}

template<class T>
void CFIR<T>::intToFloat( int16_t *input, double *output, int length )
{
    int i;

    for ( i = 0; i < length; i++ ) {
        output[i] = (double)input[i];
    }
}

template<class T>
void CFIR<T>::floatToInt( double *input, int16_t *output, int length )
{
    int i;

    for ( i = 0; i < length; i++ ) {
        if ( input[i] > 32767.0 ) {
            input[i] = 32767.0;
        } else if ( input[i] < -32768.0 ) {
            input[i] = -32768.0;
        }
        // convert
        output[i] = (int16_t)input[i];
    }
}

template class CFIR<int>;
template class CFIR<short>;
template class CFIR<double>;
