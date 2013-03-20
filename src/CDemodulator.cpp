#include "generic/complex.h"
#include "functions/idft.h"
#include <math.h>
#include <QDebug>
#ifdef FFTW
    #include <fftw3.h>
#endif
#include "CDemodulator.h"

CDemodulator::CDemodulator(QObject *parent) :
    QObject(parent)
{
    // create acarsd
    fftw = new CFFT(this,1024);
    list.append(new IDemodulator()); // 0 just dummy
    list.append(new IDemodulator()); // 1 Channel
    list.append(new IDemodulator()); // 2 Channel
    scope = 0;
    bufferBlock = 0;
}

CDemodulator::~CDemodulator()
{
    delete demodulator;
    delete fftw;
    delete data8bitsl;
    delete data16bitsl;
    delete data8bitsr;
    delete data16bitsr;
    delete xval;
    delete yval;
}

void CDemodulator::slotDataBuffer(int16_t *buffer, int size)
{
    //qDebug() << "size=" << size << " bufferBlock=" << bufferBlock;
    int channel=0, bufferSize=0, channelSize=size/2;
    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
        // Data buffer left
        //qDebug() << "copy channel position " << i << " to buffer position " << j + bufferBlock * channelSize ;
        data8bitsl[j + bufferBlock * channelSize ]  = 127.0 + (buffer[i]/32768.0)*127.0; // acars buffer on 8 bits for first channel
        data16bitsl[j + bufferBlock * channelSize ] = buffer[i]; // acars buffer on 8 bits for first channel
        // Data buffer right
        data8bitsr[j + bufferBlock * channelSize ]  = 127.0 + (buffer[i+1]/32768.0)*127.0; // acars buffer on 8 bits for first channel
        data16bitsr[j + bufferBlock * channelSize ] = buffer[i+1]; // acars buffer on 8 bits for first channel
    }
#if 1
    // Loop on each demodulator
    for (int i=1; i<list.count(); i++) {
        demodulator = (IDemodulator*)list.at(i);
        channel    = demodulator->getChannel();
        bufferSize = demodulator->getBufferSize();

        // Check if buffersize is reached for this demodulator;
        //qDebug() << "Demodulator(" << i << ") channel=" << channel << " dataSize=" << demodulator->getDataSize() << " Buffersize=" << ((bufferBlock * channelSize + channelSize) % bufferSize);
        if ((bufferSize!=0) && ((bufferBlock * channelSize + channelSize) % bufferSize == 0)) {
            //qDebug() << "Demodulator(" << i << ") channel=" << channel << " dataSize=" << demodulator->getDataSize() << " Buffersize=" << bufferBlock * channelSize + channelSize;

#if 1
            if (demodulator->getDataSize() == 8) {
                if (channel == 1)
                    demodulator->decode(data8bitsl, bufferSize, (bufferBlock * channelSize)-bufferSize); // Shift to correct buffer start
                if (channel == 2)
                    demodulator->decode(data8bitsr, bufferSize, (bufferBlock * channelSize)-bufferSize); // Shift to correct buffer start
            }
            if (demodulator->getDataSize() == 16) {
                if (channel == 1)
                    demodulator->decode(data16bitsl, bufferSize, (bufferBlock * channelSize)-bufferSize); // Shift to correct buffer start
                if (channel == 2)
                    demodulator->decode(data16bitsr, bufferSize, (bufferBlock * channelSize)-bufferSize); // Shift to correct buffer start
            }
            if (scope == time) {
                //qDebug() << "Demodulator(" << i << ") channel=" << channel << " dataSize=" << demodulator->getDataSize() << " Buffersize=" << bufferSize;//bufferBlock * channelSize + channelSize;
                if (demodulator->getDataSize() !=0 )
                {
                    for (int i=0; i < bufferSize; i++) {
                        xval[i] = i;
                        if (channel == 1)
                            yval[i] = data8bitsl[i];
                        if (channel == 2)
                            yval[i] = data8bitsr[i];
                    }
                }
            }
            bufferBlock = -1;
        }
        // Do FFT
        if (scope == fft) {
            // Do it on stereo channel
            fftw->decode(buffer,1024, xval, yval); // Shift to correct buffer start
        }
        // fill back spectrum buffer
        //if (((bufferBlock * channelSize + channelSize) % bufferSize == 0) && (demodulator->getDataSize()!=0)) {
        //}

#endif
    }
#endif
    // incrememnt block until max block size
    bufferBlock++;

    if (bufferBlock == MAXBLOCK)
        bufferBlock = 0;


    emit sigRawSamples(xval, yval, 16384);
}

void CDemodulator::slotSendData(QString data)
{
    emit sendData(data);
}

void CDemodulator::slotSetDemodulator(uint demod, uint channel, uint bufferSize)
{
    // first remove it
    delete list[channel];
    // Create the new one
    switch(demod) {
        case 0 :
            list[channel] = new IDemodulator(this); break;
        case Acars :
#ifdef WITH_ACARS
            list[channel] = new CAcars(this, channel); break;
#endif
        case AcarsGpl :
            list[channel] = new CAcarsGPL(this, channel); break;
        case CW :
            list[channel] = new CMorse(this, channel); break;
        //case RTTY :
            //demodulator = new CRtty(this);
    }
    // Connect signals
    connect(list[channel],SIGNAL(sendData(QString)), this, SLOT(slotSendData(QString)));
    // Add new demodulator to list
    //list.append(demodulator);
}

void CDemodulator::initBuffer(uint bufferSize)
{
    // Init buffers
    data8bitsl = new uchar[bufferSize]; //8bits buffer
    data16bitsl = new int16_t[bufferSize]; //16bits buffer
    data8bitsr = new uchar[bufferSize]; //8bits buffer
    data16bitsr = new int16_t[bufferSize]; //16bits buffer

    // Init ouput values
    xval = new double[bufferSize];
    yval = new double[bufferSize];
}

void CDemodulator::setScopeType(uint scope)
{
    this->scope = scope;
}
