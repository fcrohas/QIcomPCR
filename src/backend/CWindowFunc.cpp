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
#include "CWindowFunc.h"

CWindowFunc::CWindowFunc(QObject *parent) :
    QObject(parent),
    win(NULL)
{
}

void CWindowFunc::init(int size)
{
    if (win == NULL)
        win = new double[size];
    else {
        delete [] win;
        win = new double[size];
    }
    N = size;
}

void CWindowFunc::hamming()
{
    for (int i = 0; i < N; i++) {
        win[i] = 0.54 - 0.46 * cos(2*M_PI*i/(N-1));
    }
}


void CWindowFunc::hann()
{
    for (int i = 0; i < N; i++) {
        win[i] = 0.5 * (1 - cos(2*M_PI*i/(N-1)));
    }
}

void CWindowFunc::blackman()
{
    for (int i = 0; i < N; i++) {
        win[i] = 0.426591 - 0.496561*cos(2*M_PI*i/(N-1)) +0.076848*cos(4*M_PI*i/(N-1));
    }
}

void CWindowFunc::blackmanharris()
{
    for (int i = 0; i < N; i++) {
        win[i] = 0.35875 - 0.48829*cos(2*M_PI*i/(N-1)) +0.14128*cos(4*M_PI*i/(N-1))-0.01168*cos(6*M_PI*i/(N-1));
    }
}

void CWindowFunc::rectangle()
{
    for (int i = 0; i < N; i++) {
        win[i] = 1.0;
    }
}

int CWindowFunc::factorial(int n)
{
  return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

double CWindowFunc::bessel(double x)
{
    // Bessel zero order approx
    double i0 = 0.0;
    for (int k=1; k<21; k++) {
        i0 += pow(pow(x/2,k)/factorial(k),2);
    }
    i0 += 1.0;
    return i0;
}

int CWindowFunc::kaiser(double attenuation, double frequency,double width, int samplerate)
{

    double beta = 0.0;
    double M = 0.0;
    double wdelta = 2*M_PI*(frequency+width/2.0)/samplerate - 2*M_PI*(frequency-width/2.0)/samplerate;
    if ((attenuation !=0.0) && (width!=0.0)) {
        N = (attenuation - 8.0) / (4.57 * wdelta) + 1.0;
        // Update windows size according to params
        init(N);
        M=(N-1)/2;
    }
    //  beta from aa value
    if ( attenuation < 21.0) beta =0.0;
    if (( attenuation>=21.0) && (attenuation<=50.0)) beta = 0.5842*pow((attenuation-21.0),0.4)+0.07886*(attenuation-21.0);
    if (attenuation>50.0) beta = 0.1102*(attenuation-8.7);
    qDebug() << "order is " << N << " alpha is " << M << " beta is " << beta << " normalised width is " << wdelta;
    // Windowing function
    for (int n=0; n<N; n++) {
        win[n] = bessel(beta*sqrt(1.0-pow((n-M)/M,2)))/bessel(beta);
    }
    return N;
}

double CWindowFunc::get(int i)
{
    return win[i];
}

double * CWindowFunc::getWindow()
{
    return win;
}
