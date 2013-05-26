#ifndef CFIR_H
#define CFIR_H

#include <QObject>
#include <math.h>
#include <QDebug>

class CFIR : public QObject
{
    Q_OBJECT
public:
    explicit CFIR(QObject *parent = 0);
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
    double *fir;
    double *buffer;
    int N;
    double M;
    double wc;
    double fs;
};

#endif // CFIR_H
