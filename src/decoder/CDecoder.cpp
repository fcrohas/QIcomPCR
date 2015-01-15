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
#include <math.h>
#include <QDebug>
#include "CDecoder.h"

CDecoder::CDecoder(QObject *parent) :
    QThread(parent),
    xval(NULL),
    yval(NULL),
    data8bitsl(NULL),
    data8bitsr(NULL),
    data16bitsl(NULL),
    data16bitsr(NULL),
    selectedChannel(0)
{
    fftw = new CFFT(this,FFTSIZE);
    list.append(new IDecoder()); // 0 just dummy
    list.append(new IDecoder()); // 1 Channel
    list.append(new IDecoder()); // 2 Channel
    scope = 0;
    bufferBlock = 0;
}

CDecoder::~CDecoder()
{
    delete fftw;
    if (data8bitsl)
        delete [] data8bitsl;
    if (data16bitsl)
        delete [] data16bitsl;
    if (data8bitsr)
        delete [] data8bitsr;
    if (data16bitsr)
        delete [] data16bitsr;
    if (xval)
        delete [] xval;
    if (yval)
        delete [] yval;

}

//void CDecoder::slotDataBuffer(int16_t *buffer, int size)
void CDecoder::doWork()
{
    if (buffer == NULL)
        return;
    if ((data8bitsl == NULL) || (data8bitsr==NULL))
        return;
    int channel=0, bufferSize=0, channelSize=size/2;
    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
        // Data buffer left
        //qDebug() << "size=" << size << " bufferBlock=" << bufferBlock << " channelSize=" << 127.0 + (buffer[i]/32768.0)*127.0;
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
        demodulator = (IDecoder*)list.at(i);
        if (!demodulator) return;
        channel    = demodulator->getChannel();
        bufferSize = demodulator->getBufferSize();

        if ((bufferSize!=0))
            // Check if buffersize is reached for this demodulator;
            //qDebug() << "Demodulator(" << i << ") channelSize=" << channelSize << " dataSize=" << demodulator->getDataSize() << " bufferBlock=" << bufferBlock <<" bufferSize=" << bufferSize << " buffer=" << ((bufferBlock * channelSize + channelSize) % bufferSize);
        if ((bufferSize!=0) && ((bufferBlock * channelSize + channelSize)  == bufferSize)) {
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
                    for (int j=0; j < bufferSize ; j++) {
                        xval[j] = j;
                        if (channel == 1)
                            yval[j] = data8bitsl[j];
                        if (channel == 2)
                            yval[j] = data8bitsr[j];
                    }
                    emit sigRawSamples(xval, yval, bufferSize);
                }
            }
            //bufferBlock = 0;
            bufferBlock = -1;
        }
        // Do FFT
        if (scope == fft) {
            // Do it on stereo channel
            fftw->decode(buffer, size, xval, yval); // Shift to correct buffer start, do FFT on 512 bins
            emit sigRawSamples(xval, yval, FFTSIZE); // Only 256 usable so 128 per channel

        }
        // fill back spectrum buffer
        //if (((bufferBlock * channelSize + channelSize) % bufferSize == 0) && (demodulator->getDataSize()!=0)) {
        //}

#endif
    }
#endif
    // incrememnt block until max block size
    bufferBlock++;

    if ((bufferBlock * channelSize + channelSize) > 32768)
        bufferBlock = 0;
}

void CDecoder::slotSendData(QString data)
{
    emit sendData(data);
}

void CDecoder::slotSetChannel(int channel)
{
    selectedChannel = channel;
}

void CDecoder::slotSetDemodulator(uint demod, uint channel, uint bufferSize)
{
    // first remove it
    selectedChannel = channel;
    delete list[channel];
    // Create the new one
    switch(demod) {
        case 0 :
            list[channel] = new IDecoder(this); break;
        case Acars :
#ifdef WITH_ACARS
            list[channel] = new CAcars(this, channel); break;
#endif
        case AcarsGpl :
            list[channel] = new CAcarsGPL(this, channel); break;
        case CW :
            list[channel] = new CMorse(this, channel); break;
        case RTTY :
            list[channel] = new CRtty(this, channel); break;
    }
    // Connect signals
    connect(list[channel],SIGNAL(sendData(QString)), this, SLOT(slotSendData(QString)));
    // Add new demodulator to list
    //list.append(demodulator);
}

void CDecoder::initBuffer(uint bufferSize)
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

void CDecoder::setScopeType(uint scope)
{
    this->scope = scope;
}

IDecoder *CDecoder::getDemodulatorFromChannel(int channel)
{
    if (list[channel] != NULL)
        return (IDecoder*)list[channel];
    else
        return NULL;
}

void CDecoder::slotThreshold(int value)
{
    getDemodulatorFromChannel(1)->setThreshold(value);
}

void CDecoder::slotSetCorrelationLength(int value)
{
    getDemodulatorFromChannel(1)->setCorrelationLength(value);
}

void CDecoder::slotChangeWindowFunction(CFFT::windowFunction fct)
{
    fftw->setWindow(fct,FFTSIZE);
}

void CDecoder::setData(int16_t *buffer, int size) {
    this->buffer = buffer;
    this->size = size;
}
