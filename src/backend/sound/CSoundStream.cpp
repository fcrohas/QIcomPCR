#include "CSoundStream.h"


CSoundStream::CSoundStream(QObject *parent) :
    ISound(parent)
{
    connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
    server.listen(QHostAddress::Any, 8889);
    connected = false;
}

CSoundStream::~CSoundStream()
{
    server.close();
}

void CSoundStream::acceptConnection()
{
  client = server.nextPendingConnection();
  connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
  connected = true;
}

void CSoundStream::startRead()
{
  char buffer[1024] = {0};
  client->read(buffer, client->bytesAvailable());
}

void CSoundStream::encode(int16_t *data, int size)
{

}
