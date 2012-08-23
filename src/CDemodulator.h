#ifndef CDEMODULATOR_H
#define CDEMODULATOR_H

#include <QObject>
#include "CAcars.h"
#include "CAcarsGPL.h"
#include "CFft.h"

// x is sample index and f frequency
#define SAMPLERATE 22050
#define FREQ_SPACE 10000
#define FREQ_MARK  1000
#define FFTW // Use FFTW library for fourrier transform, else use SPUC

class CDemodulator : public QObject
{
    Q_OBJECT
public:
    explicit CDemodulator(QObject *parent = 0);
    ~CDemodulator();
    enum availableDemodulator {Acars=0, AcarsGpl=1, CW=2, RTTY=3};

signals:
    void sigRawSamples(double *xval, double *yval, int size);
    void sendData(QString msg);

public slots:
    void slotDataBuffer(int16_t *buffer, int size);
    void slotSendData(QString data);
    void slotSetDemodulator(uint demod);
private:
    double *space_i;
    double *space_q;
    double *mark_i;
    double *mark_q;
    double *xval;
    double *yval;
    uchar *data;
    int correlationLength;

    // Acars demodulator
    CAcars *acarsd;
    CAcarsGPL *acarsdGPL;
    CFFT *fft;
    int acarsBuffer;
};

#endif // CDEMODULATOR_H

