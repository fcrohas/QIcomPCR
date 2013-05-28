#ifndef CRTTY_H
#define CRTTY_H

#include <QObject>
#include <IDemodulator.h>
#include <math.h>
#include <qdebug.h>
#include <CWindowFunc.h>
#include <CFir.h>

#define SAMPLERATE 22050
#define STOPBITS 1.5
#define STARTBITS 1

class CRtty : public IDemodulator
{
    Q_OBJECT
public:
    explicit CRtty(QObject *parent = 0, uint channel=1);
    ~CRtty();
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
    void slotBandwidth(double value);

private:
    int bandwidth;
    int channel;
    double frequency; // Center frequency
    double freqlow; // Low mark frequency
    double freqhigh; // High space frequency
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
    double *yval;
    double *corrmark;
    double *corrspace;
    double *avgcorr;
    double *mark_q;
    double *mark_i;
    double *space_i;
    double *space_q;

    // Filters
    CWindowFunc *winfunc;
    CFIR *fmark;
    CFIR *fspace;
    CFIR *flow;

    // Goertzel filter
    double goertzel(double *x, int N, double frequency, int samplerate);

    // Correlation length generator
    void GenerateCorrelation(int length);

};

#endif // CRTTY_H
