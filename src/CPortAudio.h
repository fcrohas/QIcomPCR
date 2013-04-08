#ifndef CPORTAUDIO_H
#define CPORTAUDIO_H

#include "ISound.h"
#ifdef WITH_PORTAUDIO
#include <portaudio.h>
#endif


#define SAMPLERATE 22050
#define BUFFER_SIZE 256


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
};

#endif // CPORTAUDIO_H
