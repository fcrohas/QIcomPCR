#ifndef CFFT_H
#define CFFT_H

#include <QObject>
#include "generic/complex.h"
#include "functions/idft.h"
#include <math.h>
#include <stdint.h>
#include <QDebug>
#ifdef FFTW
    #include <fftw3.h>
#endif


class CFFT : public QObject
{
    Q_OBJECT
public:
    explicit CFFT(QObject *parent = 0, int size=2048);
    ~CFFT();
    void decode(int16_t *buffer, int size, double *xval, double *yval);
    void initBuffer(int size);
    void initFFT(int size);
signals:
    void sigRawSamples(double *xval, double *yval, int size);
    
public slots:
     void slotDataBuffer(int16_t *buffer, int size);

private:
     double *xval;
     double *yval;
#ifdef FFTW
    fftw_complex *in1, *in2;
    fftw_complex *out1, *out2;
    fftw_plan ch1, ch2;
#else
    SPUC::complex<double> *in1;
    SPUC::complex<double> *in2;
#endif
    
};

#endif // CFFT_H
