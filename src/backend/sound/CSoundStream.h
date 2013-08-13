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
#ifndef CSOUNDSTREAM_H
#define CSOUNDSTREAM_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <pa_ringbuffer.h>
#include <stdint.h>
#include "consts.h"
#ifdef WITH_SPEEX
#include <speex/speex.h>
#include <speex/speex_resampler.h>
#define DOWNSAMPLE 11025
#endif

class CSoundStream : public QObject
{
    Q_OBJECT
public:
    explicit CSoundStream(QObject *parent = 0);
    ~CSoundStream();
    void encode(int16_t *data, int size);
    void setData(int16_t *data, int size);
    void setChannel(uint value);

public slots:
    void acceptConnection();
    void startRead();
    void disconnected();
    void doWork();

private:
    QTcpServer *server;
    QTcpSocket* client;
    bool connected;
    uint channel;
#ifdef WITH_SPEEX
    SpeexResamplerState *mspeex_src_state;
    SpeexBits bits;
    void *enc_state;
    char *byte_ptr;
    int nbBytes;
    int16_t *speexBuffer;
    int16_t *stereoBuffer;
    int16_t *resampler;
    bool resample;
    // Frame size for speex
    int frame_size;
    int offset;
    int lastpos;
    int speexPos;
    int speexSize;
    int stereoPos;
    int stereoSize;
#endif
    PaUtilRingBuffer ringBuffer;
    int16_t *ringBufferData;
};

#endif // CSOUNDSTREAM_H
