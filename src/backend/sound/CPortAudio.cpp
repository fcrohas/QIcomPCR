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
#include "CPortAudio.h"

extern "C" {
    static int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
    {
        /* Cast data passed through stream to our structure. */
        CPortAudio *This = (CPortAudio *) userData;
        //(void) outputBuffer;		/* Prevent unused variable warning. */
        long bytes = frameCount*2; // 256 Frames * 2 channels
        long avail = PaUtil_GetRingBufferWriteAvailable(&This->ringBuffer);
        PaUtil_WriteRingBuffer(&This->ringBuffer, inputBuffer, (avail<bytes)?avail:bytes);
        return paContinue;
    }

    static int playbackCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
    {
        CPortAudio *This = (CPortAudio *) userData;
        int16_t *out = (int16_t*)outputBuffer;
        long bytes = frameCount*2;
        memset(out, 0, bytes);
        (void) inputBuffer;		/* Prevent unused variable warning. */
        int avail = PaUtil_GetRingBufferReadAvailable(&This->ringBuffer);
        PaUtil_ReadRingBuffer(&This->ringBuffer,out,(avail<bytes)?avail:bytes);
        This->DecodeBuffer((int16_t*)out, bytes);
    }
}


CPortAudio::CPortAudio(QObject *parent) :
    ISound(parent)
  ,ringBufferData(NULL)
{
    running = true;
    int error = Pa_Initialize();
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_Initialize error: %1\n").arg(Pa_GetErrorText( error ) );
    // build device list
    getDeviceList();
}

CPortAudio::~CPortAudio()
{
    int error;
    if (stream != NULL)
    {
        error = Pa_StopStream(stream);
        if( error != paNoError )
           qDebug() <<   QString("PortAudio Pa_StopStream error: %1\n").arg(Pa_GetErrorText( error ) );
        error = Pa_Terminate();
        if( error != paNoError )
           qDebug() <<   QString("PortAudio Pa_Terminate error: %1\n").arg(Pa_GetErrorText( error ) );
    }
    if (ringBufferData)
        delete [] ringBufferData;
}

void CPortAudio::Record(QString &filename, bool start)
{
    ISound::Record(filename, start);
}

void CPortAudio::Initialize()
{
    int error;
    qDebug() << "Portaudio Thread run() " << inputParameters.device;
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultHighOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    error = Pa_IsFormatSupported( &inputParameters, NULL, SAMPLERATE );
    if( error != paNoError )
        qDebug() <<   QString("PortAudio Pa_IsFormatSupported inputParameters error: %1\n").arg(Pa_GetErrorText( error ) );
    //error = Pa_IsFormatSupported( NULL, &outputParameters, SAMPLERATE );
    //if( error != paNoError )
    //    qDebug() <<   QString("PortAudio Pa_IsFormatSupported outputParameters error: %1\n").arg(Pa_GetErrorText( error ) );
    error = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLERATE, FRAME_SIZE, paNoFlag, recordCallback, (void *) this);
    if( error != paNoError ) {
        qDebug() <<   QString("PortAudio Pa_OpenStream input & output error: %1\n").arg(Pa_GetErrorText( error ) );
        // Open for record only
        error = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLERATE, FRAME_SIZE, paNoFlag, recordCallback, (void *) this);
        if( error != paNoError )
            qDebug() <<   QString("PortAudio Pa_OpenStream input error: %1\n").arg(Pa_GetErrorText( error ) );
    }
/*
    error = Pa_OpenStream(&stream, NULL, &outputParameters, SAMPLERATE, FRAME_SIZE, paNoFlag, playbackCallback, (void *) this);
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_OpenStream output error: %1\n").arg(Pa_GetErrorText( error ) );
*/
    if (ringBufferData)
        delete[] ringBufferData;
    ringBufferData = new int16_t[524288];
    PaUtil_InitializeRingBuffer(&ringBuffer, sizeof(int16_t) , 524288, ringBufferData);

    error = Pa_StartStream(stream);
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_StartStream error: %1\n").arg(Pa_GetErrorText( error ) );
}

void CPortAudio::run()
{
    Initialize();
    // Sound streaming
    soundStream = new CSoundStream();
    audioEncode = new QThread();
    soundStream->moveToThread(audioEncode);
    audioEncode->start();
    // Work on RingBuffer until end
    int16_t *data = new int16_t[BUFFER_SIZE];
    memset(data,0,BUFFER_SIZE);
    while(running) {
        while(PaUtil_GetRingBufferReadAvailable(&ringBuffer)<BUFFER_SIZE) { Pa_Sleep(10); }
        int readCount = PaUtil_ReadRingBuffer(&ringBuffer,data,BUFFER_SIZE);
        if (readCount<BUFFER_SIZE) qDebug() << "readcount is " << readCount;
        DecodeBuffer(data,readCount);
        // add data to ringbuffer of the encoder
        soundStream->setData(data,readCount);
        // Start the workker function;
        QMetaObject::invokeMethod(soundStream, "doWork");
    }
    delete [] data;
    audioEncode->terminate();
    delete soundStream;
    delete audioEncode;
    terminate();
}

void CPortAudio::terminate()
{
    int error;
    error = Pa_StopStream(stream);
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_StopStream error: %1\n").arg(Pa_GetErrorText( error ) );
    error = Pa_Terminate();
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_Terminate error: %1\n").arg(Pa_GetErrorText( error ) );
}

void CPortAudio::setRunning(bool value)
{
    running = value;
}

void CPortAudio::DecodeBuffer(int16_t *buffer, int size)
{
    ISound::DecodeBuffer(buffer,size);
}

QHash<QString,int> CPortAudio::getDeviceList()
{
    int error;
    int numDevices;
    deviceList.clear();
    Pa_Terminate();
    Pa_Initialize();
    numDevices = Pa_GetDeviceCount();
    qDebug() << "device count " << numDevices;
    if( numDevices < 0 )
    {
        error = numDevices;
        qDebug() <<   QString("PortAudio Pa_CountDevices error: %1\n").arg(Pa_GetErrorText( error ) );
        return deviceList;
    }
    const PaDeviceInfo *deviceInfo;
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
        qDebug() << QString("device name %1").arg(deviceInfo->name);
        deviceList.insert(deviceInfo->name, i);
    }
    return deviceList;
}

void CPortAudio::selectInputDevice(QString device)
{
    inputParameters.device = deviceList.value(device);
}

void CPortAudio::selectOutputDevice(QString device)
{
    outputParameters.device = deviceList.value(device);
}

bool CPortAudio::Load(QString &fileName)
{
    return false;
}

void CPortAudio::setChannel(uint value)
{
    if (soundStream != NULL) {
        soundStream->setChannel(value);
    }
}
