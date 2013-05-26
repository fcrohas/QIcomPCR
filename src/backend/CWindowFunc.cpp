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

double CWindowFunc::get(int i)
{
    return win[i];
}

double * CWindowFunc::getWindow()
{
    return win;
}
