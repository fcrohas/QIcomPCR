#ifndef CPULSESOUND_H
#define CPULSESOUND_H

#include <QObject>
#include <QThread>
#include <pulse/simple.h>
#include <pulse/error.h>

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
    uint8_t buffer[1024];
    pa_simple *sout;
    pa_simple *sin;
    pa_sample_spec ss;
    int error;

    // Thread
    void run();
    bool running;
};

#endif // CPULSESOUND_H
