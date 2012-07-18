#include "CPulseSound.h"
#include <QDebug>

CPulseSound::CPulseSound(QObject *parent) :
    QThread(parent)
{
    running = true;
    ss.format = PA_SAMPLE_S16NE;
    ss.channels = 2;
    ss.rate = 22000;

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

    // Setup In stream
    sin = pa_simple_new(NULL,             // Use the default server.
                      "QIcomPcr",         // Our application's name.
                      PA_STREAM_RECORD,
                      NULL,               // Use the default device.
                      "Radio In",         // Description of our stream.
                      &ss,                // Our sample format.
                      NULL,               // Use default channel map
                      NULL,               // Use default buffering attributes.
                      NULL                // Ignore error code.
                      );
    volume = new pa_cvolume();
    volume = pa_cvolume_init(volume);
    qDebug() << "Channels ? " << volume->channels;
    qDebug() << "Volume 0 ? " << volume->values[0];
    qDebug() << "Volume 1 ? " << volume->values[1];
    volume->values[0] = 10;
    volume->values[1] = 0;
    pa_cvolume_set(volume, 1, 20);
}

CPulseSound::~CPulseSound()
{
    pa_simple_free(sout);
    pa_simple_free(sin);
}

void CPulseSound::run()
{
    while(running) {
        // Read data from PCR USB Sound Card into buffer
        pa_simple_read(sin,buffer,sizeof(buffer), &error);
        // Push data to default Sound Card
        pa_simple_write(sout,buffer,sizeof(buffer),&error);
    }
}

