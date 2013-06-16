#ifndef CFFT_H
#define CFFT_H

#include <QObject>
#include "generic/complex.h"
#include "functions/idft.h"
#include "CWindowFunc.h"
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
