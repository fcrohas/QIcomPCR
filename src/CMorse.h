#ifndef CMORSE_H
#define CMORSE_H

#include <QObject>
#include <IDemodulator.h>
#include <math.h>

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
    
public slots:

private:
    double *space_i;
    double *space_q;
    double *mark_i;
    double *mark_q;
    int correlationLength;
    uint channel;
};

#endif // CMORSE_H
