#ifndef CSOUNDSTREAM_H
#define CSOUNDSTREAM_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "ISound.h"
#include "consts.h"

class CSoundStream : public ISound
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
    QTcpServer server;
    QTcpSocket* client;
    bool connected;
};

#endif // CSOUNDSTREAM_H
