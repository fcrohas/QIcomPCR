#ifndef CSOUNDSTREAM_H
#define CSOUNDSTREAM_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <stdint.h>
#include "consts.h"
#ifdef WITH_SPEEX
#include <speex/speex.h>
#endif

class CSoundStream : public QObject
{
    Q_OBJECT
public:
    explicit CSoundStream(QObject *parent = 0);
    ~CSoundStream();
    void encode(int16_t *data, int size);

public slots:
    void acceptConnection();
    void startRead();

private:
    QTcpServer *server;
    QTcpSocket* client;
    bool connected;
#ifdef WITH_SPEEX
    SpeexBits bits;
    void *enc_state;
    char *byte_ptr;
    int nbBytes;
    int16_t *audiol;
    int16_t *audior;
#endif

};

#endif // CSOUNDSTREAM_H
