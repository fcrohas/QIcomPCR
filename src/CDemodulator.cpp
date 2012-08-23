#include "generic/complex.h"
#include "functions/idft.h"
#include <math.h>
#include <QDebug>
#include <fftw3.h>
#include "CDemodulator.h"

CDemodulator::CDemodulator(QObject *parent) :
    QObject(parent)
{
    // Init sinus and cos for correlation
    correlationLength = 22050 /1200; // For 1200 baud
    space_i = new double[correlationLength];
    space_q = new double[correlationLength];
    mark_i  = new double[correlationLength];
    mark_q  = new double[correlationLength];
    double f = 0.0;
    for (int i=0; i< correlationLength;i++) {
        space_i[i] = cos(f);
        space_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_SPACE/SAMPLERATE;
        //qDebug() << "space[" << i << "]=" << space_i[i];

    }
    f = 0.0;
    for (int i=0; i< correlationLength;i++) {
        mark_i[i] = cos(f);
        mark_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_MARK/SAMPLERATE;
    }
    // Init ouput values
    xval = new double[16384];
    yval = new double[16384];
    // create acarsd
    acarsd = new CAcars(this);
    connect(acarsd,SIGNAL(sendData(QString)), this, SLOT(slotSendData(QString)));
    acarsdGPL = new CAcarsGPL(this);
    fft = new CFFT(this,16384);
    data = new uchar[16384]; //Big acars buffer to decode
    acarsBuffer = 0;
}

CDemodulator::~CDemodulator()
{
    delete acarsd;
    delete acarsdGPL;
    delete fft;
    delete data;
    delete xval;
    delete yval;
}

void CDemodulator::slotDataBuffer(int16_t *buffer, int size)
{
    //fft->decode(buffer,size, xval, yval);

    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
        data[j+acarsBuffer * 1024]   = 127.0 + (buffer[i]/32768.0)*127.0; // acars buffer on 8 bits for first channel
        /////data[j+acarsBuffer * 1024]   = buffer[i]; // acars buffer on 8 bits for first channel
    }

    //if (acarsBuffer == 2048/1024-1 ) {
        //acarsdGPL->decode(buffer,size);
        acarsd->decode(data, size/2);
        //acarsBuffer = 0;
    //} else acarsBuffer++;

    // fill back spectrum buffer
    for (int i=0; i < size/2; i++) {
        xval[i] = i;
        yval[i] = data[i];
    }

    emit sigRawSamples(xval, yval, size/2);
}

void CDemodulator::slotSendData(QString data)
{
    emit sendData(data);
}

void CDemodulator::slotSetDemodulator(uint demod)
{
    /*
    switch(demod) {
        case Acars :
        case AcarsGpl :
        case CW :
        case RTTY :
    }
    */
}
