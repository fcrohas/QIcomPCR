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

public slots:
    void acceptConnection();
    void startRead();
    void disconnected();
    void doWork();

private:
    QTcpServer *server;
    QTcpSocket* client;
    bool connected;
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
