#include "CPulseSound.h"

#include <QDebug>

CPulseSound::CPulseSound(QObject *parent) :
    ISound(parent)
{
    running = true;
    ss.format = PA_SAMPLE_S16LE; // 16 bits sample
    ss.channels = 2;
    ss.rate = SAMPLERATE;
    //buffer_attr.fragsize = buffer_attr.tlength = pa_usec_to_bytes(latency_msec * PA_USEC_PER_MSEC, &sample_spec);
    attr.maxlength = -1;
    attr.tlength = pa_usec_to_bytes(1000, &ss);
    attr.fragsize = pa_usec_to_bytes(1000, &ss);
    attr.tlength = -1;
    attr.prebuf = -1;
    map.channels = 2;
    map.map[0] = PA_CHANNEL_POSITION_FRONT_LEFT;
    map.map[1] = PA_CHANNEL_POSITION_FRONT_RIGHT;
/*
    // Setup Out stream
    soundOut = pa_simple_new(NULL,            // Use the default server.
                      "QIcomPcr",         // Our application's name.
                      PA_STREAM_PLAYBACK,
                      NULL,               // Use the default device.
                      "Radio Out",        // Description of our stream.
                      &ss,                // Our sample format.
                      &map,               // Use default channel map
                      &attr,               // Use default buffering attributes.
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
    if (soundIn)
        pa_simple_free(soundIn);
    if (soundOut)
        pa_simple_free(soundOut);
}

void CPulseSound::run()
{
    int error;
    while(running) {
        ssize_t r;

        pa_usec_t latency;
/*
        if ((latency = pa_simple_get_latency(soundIn, &error)) == (pa_usec_t) -1) {
            running = false;
        }

        qDebug() << QString("In:  %1 usec    \r\n").arg((float)latency);
        //fprintf(stderr, "In:  %0.0f usec    \r\n", (float)latency);

        if ((latency = pa_simple_get_latency(soundOut, &error)) == (pa_usec_t) -1) {
            running = false;
        }

        qDebug() << QString("Out:  %1 usec    \r\n").arg((float)latency);
        //fprintf(stderr, "Out: %0.0f usec    \r\n", (float)latency);
*/
        if (pa_simple_read(soundIn, buffer, sizeof(buffer), &error) < 0) {
            running = false;
        }

        /* ... and play it */
/*        if (pa_simple_write(soundOut, buffer, sizeof(buffer), &error) < 0) {
            running = false;
        }*/
        DecodeBuffer(buffer, BUFFER_SIZE);
        //emit dataBuffer(buffer, BUFFER_SIZE);
    }

    /* Make sure that every single sample was played */
    pa_simple_drain(soundOut, &error);
}

void CPulseSound::setRunning(bool value)
{
    running = value;
}

void CPulseSound::DecodeBuffer(int16_t *buffer, int size)
{
    ISound::DecodeBuffer(buffer,size);
}
