#ifndef CMORSE_H
#define CMORSE_H

#include <QObject>
#include <IDemodulator.h>
#include <math.h>
#include "functions/window.h"

// x is sample index and f frequency
#define SAMPLERATE 22050
#define FREQ_SPACE 10000
#define FREQ_MARK  1000


class CMorse : public IDemodulator
{
    Q_OBJECT
public:
    explicit CMorse(QObject *parent = 0, uint channel=1);
    void decode(int16_t *buffer, int size, int offset);
    uint getDataSize();
    uint getChannel();
    uint getBufferSize();
    
signals:
    void dumpData(double*,double*,int);
public slots:
    void slotFrequency(int value);

private:
    double *space_i;
    double *space_q;
    double *mark_i;
    double *mark_q;
    double *window;
    double *xval;
    double *yval;
    double *corr;
    int correlationLength;
    uint channel;
    int frequency;
};

#endif // CMORSE_H
