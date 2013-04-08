#include "CPortAudio.h"

extern "C" {
    static int recordCallback(const void* inputBuffer, void* outputBuffer, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void* userData)
    {
        /* Cast data passed through stream to our structure. */
        //int16_t *out = (int16_t*)outputBuffer;
        //int16_t *in = (int16_t*)inputBuffer;
        CPortAudio *This = (CPortAudio *) userData;
        (void) outputBuffer;		/* Prevent unused variable warning. */
        //out = in;
        This->DecodeBuffer((int16_t*)inputBuffer, BUFFER_SIZE);
        return paContinue;
    }
}


CPortAudio::CPortAudio(QObject *parent) :
    ISound(parent)
{
    running = true;
    int error = Pa_Initialize();
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_Initialize error: %1\n").arg(Pa_GetErrorText( error ) );
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
}

void CPortAudio::Record(QString &filename, bool start)
{

}

void CPortAudio::run()
{
    int error;
    qDebug() << "Portaudio Thread run()";
    outputParameters.channelCount = 2;       /* stereo output */
    outputParameters.sampleFormat = paInt16;
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
    inputParameters.channelCount = 2;
    inputParameters.sampleFormat = paInt16;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;
    error = Pa_IsFormatSupported( &inputParameters, NULL, SAMPLERATE );
    if( error != paNoError )
        qDebug() <<   QString("PortAudio Pa_IsFormatSupported inputParameters error: %1\n").arg(Pa_GetErrorText( error ) );
    error = Pa_IsFormatSupported( NULL, &outputParameters, SAMPLERATE );
    if( error != paNoError )
        qDebug() <<   QString("PortAudio Pa_IsFormatSupported outputParameters error: %1\n").arg(Pa_GetErrorText( error ) );
    error = Pa_OpenStream(&stream, &inputParameters, NULL, SAMPLERATE, BUFFER_SIZE, paNoFlag, recordCallback, (void *) this);
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_OpenStream error: %1\n").arg(Pa_GetErrorText( error ) );
    error = Pa_StartStream(stream);
    if( error != paNoError )
       qDebug() <<   QString("PortAudio Pa_StartStream error: %1\n").arg(Pa_GetErrorText( error ) );
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
    numDevices = Pa_GetDeviceCount();
    if( numDevices < 0 )
    {
        //qDebug() << QString("ERROR: Pa_CountDevices returned 0x%1\n").arg(numDevices );
        error = numDevices;
        qDebug() <<   QString("PortAudio Pa_CountDevices error: %1\n").arg(Pa_GetErrorText( error ) );
        return deviceList;
    }
    const PaDeviceInfo *deviceInfo;
    for( int i=0; i<numDevices; i++ )
    {
        deviceInfo = Pa_GetDeviceInfo( i );
        deviceList.insert(deviceInfo->name, i);
        //qDebug() << QString("%2 deviceInfo: %1\n").arg(deviceInfo->name ).arg(i);
        //qDebug() << QString("%2 device hostApi: %1\n").arg(Pa_GetHostApiInfo( deviceInfo->hostApi )->name  ).arg(i);
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
