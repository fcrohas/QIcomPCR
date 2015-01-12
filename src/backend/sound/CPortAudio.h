/**********************************************************************************************
  Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
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
    explicit CPortAudio(QObject *parent = 0, ISound::Mode mode = eRecord);
    ~CPortAudio();
    bool Load(QString &fileName);
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);
    QHash<QString, int> getDeviceList();
    void Record(QString &filename, bool start);
    PaUtilRingBuffer ringBuffer;
    void Play(int16_t *buffer, int size);

signals:
    
public slots:
    void setChannel(uint value);

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
    QThread *audioEncode;
    QThread *dataDecode;
    int mode;

};

#endif // CPORTAUDIO_H
