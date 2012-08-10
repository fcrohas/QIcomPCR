#ifndef CPULSESOUND_H
#define CPULSESOUND_H

#include <QObject>
#include <QThread>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/volume.h>
#include "ui_CMainWindow.h"

#define BUFFER_SIZE 256

class CPulseSound : public QThread
{
    Q_OBJECT
public:
    explicit CPulseSound(QObject *parent = 0);
    ~CPulseSound();

signals:
    void dataBuffer(double *xval, double *yval);
    
public slots:

private:
    // PulseAudio
    uint16_t buffer[BUFFER_SIZE]; // 16 bits pulse audio buffer
    pa_simple *sout;
    pa_simple *sin;
    pa_sample_spec ss;
    pa_buffer_attr attr;
    pa_channel_map map;
    pa_cvolume *volume;
    int error;
    // Correlator FSK
    uint16_t sinf1[BUFFER_SIZE];
    uint16_t sinf2[BUFFER_SIZE];
    uint16_t cosf1[BUFFER_SIZE];
    uint16_t cosf2[BUFFER_SIZE];
    // Thread
    void run();
    bool running;
    void decodePOCSag(uint16_t buffer[]);
};

#endif // CPULSESOUND_H

