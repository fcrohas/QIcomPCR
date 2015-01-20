#ifndef CDEMODULATORBASE_H
#define CDEMODULATORBASE_H

#include <QObject>
#include <QMutex>
#include "ISound.h"

#ifdef WITH_SAMPLERATE
#include <samplerate.h>
#endif

class CDemodulatorBase : public QObject
{
    Q_OBJECT
public:
    enum Mode { eLSB=0, eUSB=1, eAM=2, eCW=3, eUkn=4, eFM=5, eWFM=6};
    explicit CDemodulatorBase(QObject *parent = 0, CDemodulatorBase::Mode mode = CDemodulatorBase::eUkn);
    ~CDemodulatorBase();
    void setSoundDevice(ISound *device);
    void setData(int16_t *buffer,int len);
    void processSound(int16_t *buffer,int len);
    // Samplerate
    void setSampleRate(uint frequency);
    // Down sample filter
    int downsample(int16_t *buffer, int len, int factor = 0);
    // Resample
    int resample(int16_t *buffer, int len, int samplerate);
    // Low pass resample
    int low_pass_real(int16_t *buffer, int len);
    // RMS power for snr
    int rms(int step);
    // MAD power
    int mad(int step);
    //
    QMutex update;
    // Demodulator name
    virtual QString getName();
signals:
    void finished();

public slots:
    void doWork();
    void slotSetFilter(uint frequency);

private:
    // samplerate
    uint samplerate;
    // simple downsample internal
    int prev_index;
    int now_r;
    int now_j;
    // sound device
    ISound *sound;
    // Sample rate conversion
    SRC_STATE* converter;
    SRC_DATA dataconv;
    // resample buffer
    float *inputbufferf;
    float *outputbufferf;

protected:
    Mode mode;
    int filterfreq;
    // decimation 1st stage
    int decimation;
    // intermediate sampling frequency after 1st stage
    int intfreq;
    // internal thread buffer
    int16_t *buffer;
    int len;
};

#endif // CDEMODULATORBASE_H
