#ifndef CDEMODULATOR_H
#define CDEMODULATOR_H

#include <QObject>
#include "IDemodulator.h"
#ifdef WITH_ACARS
    #include "CAcars.h"
#endif
#include "CAcarsGPL.h"
#include "CMorse.h"
#include "CFft.h"

//#define FFTW // Use FFTW library for fourrier transform, else use SPUC
#define MAXBLOCK 64 // for a max buffer of 32768 and sound buffer of 512 per channel

class CDemodulator : public QObject
{
    Q_OBJECT
public:
    explicit CDemodulator(QObject *parent = 0);
    ~CDemodulator();
    enum availableDemodulator {Acars=1, AcarsGpl=2, CW=3, RTTY=4};
    void initBuffer(uint bufferSize);
    enum scopeType {time=0, fft=1, waterfall=2};
    void setScopeType(uint scope);
    IDemodulator* getDemodulatorFromChannel(int channel);

signals:
    void sigRawSamples(double *xval, double *yval, int size);
    void sendData(QString msg);

public slots:
    void slotDataBuffer(int16_t *buffer, int size);
    void slotSendData(QString data);
    void slotSetDemodulator(uint demod, uint channel, uint bufferSize);
    void slotThreshold(int value);
    void slotSetCorrelationLength(int value);
    void slotChangeWindowFunction(CFFT::windowFunction fct);

private:
    QList<IDemodulator*> list;
    double *xval;
    double *yval;
    int bufferBlock;
    uchar *data8bitsl;
    int16_t *data16bitsl;
    uchar *data8bitsr;
    int16_t *data16bitsr;
    uint scope;
    IDemodulator *demodulator;

    // FFT
    CFFT *fftw;
};

#endif // CDEMODULATOR_H

