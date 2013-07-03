#ifndef CPORTAUDIO_H
#define CPORTAUDIO_H

#include "ISound.h"
#include <consts.h>
#ifdef WITH_PORTAUDIO
#include <portaudio.h>
#include <pa_ringbuffer.h>
#endif
#ifdef WITH_SPEEX
#include <speex/speex.h>
#endif
#include "CSoundStream.h"

extern "C" {
    typedef struct{
        int frameIndex;
        int maxFrameIndex;
        char* recordedSamples;
    } testData;

    static int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData);
}


class CPortAudio : public ISound
{
    Q_OBJECT
public:
    explicit CPortAudio(QObject *parent = 0);
    ~CPortAudio();
    bool Load(QString &fileName);
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);
    QHash<QString, int> getDeviceList();
    void Record(QString &filename, bool start);
    PaUtilRingBuffer ringBuffer;

signals:
    
public slots:

private:

    int16_t buffer[BUFFER_SIZE]; // 16 bits pulse audio buffer
    bool running;
    void run();
    void terminate();
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    PaStream *stream = NULL;
    void *data;
    QHash<QString,int> deviceList;
    void selectInputDevice(QString device);
    void selectOutputDevice(QString device);
    void Initialize();
    int16_t *ringBufferData;
    CSoundStream *soundStream;
};

#endif // CPORTAUDIO_H
