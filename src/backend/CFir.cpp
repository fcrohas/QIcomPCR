#include "CFir.h"

CFIR::CFIR(QObject *parent) :
    QObject(parent),
    win(NULL),
    fir(NULL),
    buffer(NULL)
{

}

void CFIR::setWindow(double *value)
{
    win = value;
}

void CFIR::lowpass(double frequency)
{
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
}

void CFIR::highpass(double frequency)
{
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
}

void CFIR::bandpass(double centerfreq, double bandwidth)
{
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
}

void CFIR::stopband(double centerfreq, double bandwidth)
{
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
    if (buffer == NULL) {
        buffer = new double[size];
    }
    // loop on buffer and apply filter
    for (int i=0; i<size; i++) {
        double sample = 0.0;
        // act as a ring buffer for current and previous call
        buffer[i] = in[i];
        // direct fir filter
        for (int j=0; j<N; j++) {
            if ((i-j) < 0)
                sample += buffer[(size-1)+i-j] * fir[j]; // use previous buffer call values
            else
                sample += buffer[i-j] * fir[j]; // use current buffer values
        }
        // Save
        in[i] = sample;
    }
}
