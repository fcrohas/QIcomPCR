#ifndef CPULSESOUND_H
#define CPULSESOUND_H

#include <QObject>
#include <ISound.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <pulse/volume.h>
#include "ui_CMainWindow.h"

#define SAMPLERATE 22050
#define BUFFER_SIZE 512

class CPulseSound : public ISound
{
    Q_OBJECT
public:
    explicit CPulseSound(QObject *parent = 0);
    ~CPulseSound();
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);

signals:
    void dataBuffer(int16_t *buffer, int size);
    
public slots:

private:
    // PulseAudio
    int16_t buffer[BUFFER_SIZE]; // 16 bits pulse audio buffer
    pa_simple *soundOut;
    pa_simple *soundIn;
    pa_sample_spec ss;
    pa_buffer_attr attr;
    pa_channel_map map;
    pa_cvolume *volume;
    int error;
    // Thread
    void run();
    bool running;
};

#endif // CPULSESOUND_H


