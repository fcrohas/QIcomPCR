#include "CRemoteControl.h"

#include <iostream>

CRemoteControl::CRemoteControl(QObject* parent): QObject(parent)
{
  connect(&server, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

  server.listen(QHostAddress::Any, 8888);
  connected = false;
}

CRemoteControl::~CRemoteControl()
{
  server.close();
}

void CRemoteControl::acceptConnection()
{
  client = server.nextPendingConnection();

  connect(client, SIGNAL(readyRead()), this, SLOT(startRead()));
  connected = true;
}

void CRemoteControl::startRead()
{
  char buffer[1024] = {0};
  client->read(buffer, client->bytesAvailable());
  decode(buffer);
  //client->close();
}

void CRemoteControl::decode(char *buffer)
{
    QString decodedString(buffer);
    qDebug() << decodedString;
    emit command(decodedString);
    client->write("Connected to qicompcr");

}

void CRemoteControl::sendData(QString value)
{
    if (connected)
        client->write(value.toLocal8Bit());
}
