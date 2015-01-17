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
    if (tmin < -127) tmin = -1.0;
    tmax = std::numeric_limits<T>::max(); // maximum value
    if (tmax > 127) tmax = 1.0;
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
        if (i==M)
            fir[i] = wc/M_PI;
        else
            fir[i] = sin(wc*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
    }
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
        if (i==M)
            fir[i] = - sin(wc*(i-M))/(M_PI*(i-M));
        else
            fir[i] = 1 - wc / M_PI;
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
    }
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
        if (i==M)
            fir[i] = (wc2 - wc1)/M_PI;
        else
            fir[i] = sin(wc2*(i-M))/(M_PI*(i-M)) - sin(wc1*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
    }
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
        if (i==M)
            fir[i] = 1.0 - (wc2 - wc1)/M_PI;
        else
            fir[i] = sin(wc1*(i-M))/(M_PI*(i-M)) - sin(wc2*(i-M))/(M_PI*(i-M));
        // Apply window
        if (win != NULL)
            fir[i] = fir[i] * win [i];
    }
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
    }
    else {
        delete [] fir;
        delete [] tfir;
        tfir = new T[N];
        fir = new double[N];
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
        buffer = new T[size];
    }
    // loop on buffer and apply filter
#if 1
    for (int i=0; i<size; i++) {
        T sample = 0.0;
        // act as a ring buffer for current and previous call
        buffer[i] = in[i];
        // direct fir filter
        for (int j=0; j<N; j++) {
            if ((i-j) > 0)
                sample += buffer[size+i-j] * tfir[j]; // use previous buffer call values
            else
                sample += buffer[i-j] * tfir[j]; // use current buffer values
            //sample += in[i] * tfir[j];
        }
        // Save
        in[i] = sample;
    }
#endif
#if 0
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
    qDebug() << "scale factor is " << scaleFactor;
    if (scaleFactor == 0) scaleFactor = 1.0;
    for (int i=0; i < N; i++) {
        tfir[i] = fir[i] * scaleFactor;
    }
}

// the FIR filter function
template<class T>
void CFIR<T>::apply(int16_t *&in, int size)
{
    if (update)
        return;
    if (in == NULL) {
        return;
    }

    if (bufferd == NULL) {
        bufferd = new double[size];
    }
    // loop on buffer and apply filter
#if 1
    double *inputp;
    double acc;
    // Convert for int16_t
    intToFloat(in,bufferd,size);
    // apply the filter to each input sample
    for ( int n = 0; n < size; n++ ) {
        inputp = &bufferd[N - 1 + n];
        acc = 0;
        for ( int k = 0; k < N; k++ ) {
            acc += tfir[k] * (*inputp--);
        }
        in[n] = acc;
    }
    // convert back
    floatToInt(bufferd,in,size);
#endif
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
//template class CFIR<short>;
template class CFIR<double>;
