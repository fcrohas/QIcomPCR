#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QObject>
#include "ISound.h"
#ifdef WITH_SAMPLERATE
#include <samplerate.h>
#endif
//#define SAMPLERATE 22050

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
    int downsample(int16_t *buffer, int len, int factor = 0);
    // Resample
    int resample(int16_t *buffer, int len, int samplerate);
    bool update;
signals:
    void finished();

public slots:
    void doWork();
    void slotSetFilter(uint frequency);

private:
    ISound *sound;
protected:
    Mode mode;
    int filterfreq;
    int16_t *buffer;
    int len;
    int prev_index;
    int now_r;
    int now_j;
    int downSampleFactor;
    // resample buffer
    float *inputbufferf;
    float *outputbufferf;
    // Sample rate conversion
    SRC_STATE* converter;
    SRC_DATA dataconv;
};

#endif // IDEMODULATOR_H
