#include "CPulseSound.h"

#include <QDebug>

CPulseSound::CPulseSound(QObject *parent) :
    QThread(parent)
{
    running = true;
    ss.format = PA_SAMPLE_S16NE; // 16 bits sample
    ss.channels = 2;
    ss.rate = SAMPLERATE;
    attr.maxlength = -1;
    attr.fragsize = -1;
    attr.tlength = -1;
    attr.prebuf = -1;
    map.channels = 2;
    map.map[0] = PA_CHANNEL_POSITION_FRONT_LEFT;
    map.map[1] = PA_CHANNEL_POSITION_FRONT_RIGHT;
/*
    // Setup Out stream
    sout = pa_simple_new(NULL,            // Use the default server.
                      "QIcomPcr",         // Our application's name.
                      PA_STREAM_PLAYBACK,
                      NULL,               // Use the default device.
                      "Radio Out",        // Description of our stream.
                      &ss,                // Our sample format.
                      NULL,               // Use default channel map
                      NULL,               // Use default buffering attributes.
                      NULL                // Ignore error code.
                      );
*/
    // Setup In stream
    soundIn = pa_simple_new(NULL,             // Use the default server.
                      "QIcomPcr",         // Our application's name.
                      PA_STREAM_RECORD,
                      NULL,               // Use the default device.
                      "Radio In",         // Description of our stream.
                      &ss,                // Our sample format.
                      &map,               // Use default channel map
                      &attr,              // Use default buffering attributes.
                      NULL                // Ignore error code.
                      );

}


CPulseSound::~CPulseSound()
{
//    pa_simple_free(sout);
    running = false;
    pa_simple_free(soundIn);
}

void CPulseSound::run()
{
    while(running) {
        // Read data from PCR USB Sound Card into buffer
        pa_simple_read(soundIn,buffer,sizeof(buffer), &error);
        emit dataBuffer(buffer, BUFFER_SIZE);
    }
}

void CPulseSound::setRunning(bool value)
{
    running = value;
}


