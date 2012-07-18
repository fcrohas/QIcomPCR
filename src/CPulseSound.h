#ifndef CPULSESOUND_H
#define CPULSESOUND_H

#include <QObject>
#include <QThread>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/volume.h>

class CPulseSound : public QThread
{
    Q_OBJECT
public:
    explicit CPulseSound(QObject *parent = 0);
    ~CPulseSound();
signals:
    
public slots:

private:
    // PulseAudio
    uint8_t buffer[128];
    pa_simple *sout;
    pa_simple *sin;
    pa_sample_spec ss;
    pa_cvolume *volume;
    int error;

    // Thread
    void run();
    bool running;
};

#endif // CPULSESOUND_H
