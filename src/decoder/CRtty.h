#ifndef CRTTY_H
#define CRTTY_H

#include <QObject>
#include <IDemodulator.h>
#include <math.h>
#include <qdebug.h>
#include "dspfilters/Dsp.h"

#define SAMPLERATE 22050
#define STOPBITS 2
#define STARTBITS 1

class CRtty : public IDemodulator
{
    Q_OBJECT
public:
    explicit CRtty(QObject *parent = 0, uint channel=1);
    void decode(int16_t *buffer, int size, int offset);
    uint getDataSize();
    uint getChannel();
    uint getBufferSize();
    void setThreshold(int value) override;
    void setCorrelationLength(int value) override;

signals:
    void dumpData(double*,double*,int);

public slots:
    void slotFrequency(double value);
    void slotBandwidth(int value);

private:
    int bandwidth;
    int channel;
    int frequency; // Center frequency
    int freqlow; // Low mark frequency
    int freqhigh; // High space frequency
    int correlationLength;
    double baudrate;
    double inverse;
    int accmark;
    int accspace;
    // letter
    QString letter;
    int bitcount;
    bool started;
    int bit;

    // Audio channel buffer
    double* audioData[1];

    // Table
    double *xval;
    double *corrmark;
    double *corrspace;
    double *avgcorr;

    // Filters
    Dsp::Filter *fbandpass;
    Dsp::Filter *flowpass;

    // Goertzel filter
    double goertzel(double *x, int N, double frequency, int samplerate);

};

#endif // CRTTY_H
