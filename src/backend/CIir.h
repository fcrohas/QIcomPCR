#ifndef CIIR_H
#define CIIR_H

#include <QObject>
#include <QDebug>
#include <math.h>

class CIIR : public QObject
{
    Q_OBJECT
public:
    explicit CIIR(QObject *parent = 0);
    ~CIIR();
    void setWindow(double *win);
    void lowpass(double frequency);
    void highpass(double frequency);
    void bandpass(double centerfreq, double bandwidth);
    void stopband(double centerfreq, double bandwidth);
    void setOrder(int value);
    void setSampleRate(double value);
    void apply(double *&in, int size);

signals:
    
public slots:

private:
    double *win;
    double *iirA;
    double *iirB;
    double *bufferin;
    double *bufferout;
    double wc;
    double fs;
    // Filter order only even to calculate passes
    int N;
    // Second order coeff lowpass
    double a0l,a1l,a2l;
    double b1l,b2l;
    // Second order coeff highpass
    double a0h,a1h,a2h;
    double b1h,b2h;
    // High or low pass or both
    bool high;
    bool low;
    bool band;
    bool stop;
    bool update;
};

#endif // CIIR_H
