#include "CPulseSound.h"
#include "generic/complex.h"
#include "functions/idft.h"
#include "butterworth.h"
#include <QDebug>

// x is sample index and f frequency
#define SAMPLERATE 22050
#define FREQ_SPACE 2200
#define FREQ_MARK  1200
#define COS(x,f) x*2*PI*f/SAMPLERATE
#define SIN(x,f) x*2*PI*f/SAMPLERATE

CPulseSound::CPulseSound(QObject *parent) :
    QThread(parent)
{
    running = true;
    ss.format = PA_SAMPLE_S16NE; // 16 bits sample
    ss.channels = 1;
    ss.rate = SAMPLERATE;
    attr.maxlength = -1;
    attr.fragsize = -1;
    attr.tlength = -1;
    attr.prebuf = -1;
    map.channels = 1;
    map.map[0] = PA_CHANNEL_POSITION_MONO;
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
    sin = pa_simple_new(NULL,             // Use the default server.
                      "QIcomPcr",         // Our application's name.
                      PA_STREAM_RECORD,
                      NULL,               // Use the default device.
                      "Radio In",         // Description of our stream.
                      &ss,                // Our sample format.
                      NULL,               // Use default channel map
                      &attr,              // Use default buffering attributes.
                      NULL                // Ignore error code.
                      );
/*    volume = new pa_cvolume();
    volume = pa_cvolume_init(volume);
    qDebug() << "Channels ? " << volume->channels;
    qDebug() << "Volume 0 ? " << volume->values[0];
    qDebug() << "Volume 1 ? " << volume->values[1];
    volume->values[0] = 10;
    volume->values[1] = 0;
    pa_cvolume_set(volume, 1, 20);*/
    correlationLength = SAMPLERATE / 1200; // For 120 baud
    for (int i=0; i< correlationLength;i++) {
        space_q[i] = SIN(i,FREQ_SPACE);
        space_i[i] = COS(i,FREQ_SPACE);
        space_q[i] = SIN(i,FREQ_MARK);
        mark_i[i]  = COS(i,FREQ_MARK);
    }
}


CPulseSound::~CPulseSound()
{
//    pa_simple_free(sout);
    pa_simple_free(sin);
}

void CPulseSound::run()
{
    while(running) {
        // Read data from PCR USB Sound Card into buffer
        pa_simple_read(sin,buffer,sizeof(buffer), &error);
        decodePOCSag(buffer);
        // Push data to default Sound Card
        //pa_simple_write(sout,buffer,sizeof(buffer),&error);
    }
}

void CPulseSound::decodePOCSag(uint16_t buffer[BUFFER_SIZE])
{
    // Convert sample to complex number only real part set
    double *xval = new double[BUFFER_SIZE];
    double *yval = new double[BUFFER_SIZE];
    SPUC::complex<double> *data = new SPUC::complex<double>[BUFFER_SIZE];
    SPUC::butterworth<double> filterlp(0.65,2,3.0);
    SPUC::butterworth<double> filterhp(0.25,2,3.0);

    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        data[i].re = buffer[i]*1.0/65535; // set real between 0 and 1
        data[i].im = 0;
        data[i].re = filterlp.clock(data[i].real());
        data[i].re = filterhp.clock(data[i].real());
    }
    // Do the dft transform
    SPUC::dft(data,BUFFER_SIZE);
    for (int i = 1; i < BUFFER_SIZE; i++)
    {
        xval[i] = i;
        yval[i] = sqrt(SPUC::magsq(data[i])); // Normalized vector
    }
    emit dataBuffer(xval,yval);
    // dump dft
    // Detect frequencies
    for (int i = 1; i <128; i++)
        if (yval[i]>0.5)
          qDebug() << "freq " << 22050*i/128 << " hz magnitude=" << yval[i];
    delete xval;
    delete yval;
    delete data;

}
