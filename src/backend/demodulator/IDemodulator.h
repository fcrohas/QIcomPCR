#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QObject>
#include "ISound.h"

#define DOWNSAMPLE 8

class IDemodulator : public QObject
{
    Q_OBJECT
public:
    enum Mode { eLSB=0, eUSB=1, eAM=2, eCW=3, eUkn=4, eFM=5, eWFM=6};
    explicit IDemodulator(QObject *parent = 0, IDemodulator::Mode mode = IDemodulator::eUkn);
    void setSoundDevice(ISound *device);
    void setData(int16_t *buffer,int len);
    void processSound(int16_t *buffer,int len);
    // Down sample filter
    void downsample(int16_t *buffer, int len, int factor = 0);
signals:
    void finished();

public slots:
    void doWork();
    void slotSetFilter(uint frequency);

private:
    ISound *sound;
protected:
    int16_t *buffer;
    int len;
    int prev_index;
    int now_r;
    int now_j;
    int downSampleFactor;
};

#endif // IDEMODULATOR_H
