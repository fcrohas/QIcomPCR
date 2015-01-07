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
#ifndef CDECODER_H
#define CDECODER_H

#include <QObject>
#include "IDecoder.h"
#ifdef WITH_ACARS
    #include "CAcars.h"
#endif
#include "CAcarsGPL.h"
#include "CMorse.h"
#include "CRtty.h"
#include "CFft.h"
#include <consts.h>

//#define FFTW // Use FFTW library for fourrier transform, else use SPUC
#define MAXBLOCK 64 // for a max buffer of 32768 and sound buffer of 512 per channel

class CDecoder : public QObject
{
    Q_OBJECT
public:
    explicit CDecoder(QObject *parent = 0);
    ~CDecoder();
    enum availableDemodulator {Acars=1, AcarsGpl=2, CW=3, RTTY=4};
    void initBuffer(uint bufferSize);
    enum scopeType {time=0, fft=1, waterfall=2};
    void setScopeType(uint scope);
    IDecoder* getDemodulatorFromChannel(int channel);

signals:
    void sigRawSamples(double *xval, double *yval, int size);
    void sendData(QString msg);

public slots:
    void slotDataBuffer(int16_t *buffer, int size);
    void slotSendData(QString data);
    void slotSetDemodulator(uint demod, uint channel, uint bufferSize);
    void slotThreshold(int value);
    void slotSetCorrelationLength(int value);
    void slotChangeWindowFunction(CFFT::windowFunction fct);
    void slotSetChannel(int channel);

private:
    QList<IDecoder*> list;
    double *xval;
    double *yval;
    int bufferBlock;
    int selectedChannel;
    uchar *data8bitsl;
    int16_t *data16bitsl;
    uchar *data8bitsr;
    int16_t *data16bitsr;
    uint scope;
    IDecoder *demodulator;

    // FFT
    CFFT *fftw;
};

#endif // CDECODER_H

