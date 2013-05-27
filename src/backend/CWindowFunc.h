#ifndef CWINDOWFUNC_H
#define CWINDOWFUNC_H

#include <QObject>
#include <math.h>
#include <qdebug.h>

class CWindowFunc : public QObject
{
    Q_OBJECT
public:
    explicit CWindowFunc(QObject *parent = 0);
    void hann();
    void hamming();
    void blackman();
    void blackmanharris();
    void rectangle();
    int kaiser(double attenuation, double frequency, double width, int samplerate);
    void init(int size);
    double get(int i);
    double * getWindow();
signals:
    
public slots:

private:
    double *win;
    int N;
    int factorial(int n);
    double bessel(double x);
};

#endif // CWINDOWFUNC_H
