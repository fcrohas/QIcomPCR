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
}

void CRemoteControl::decode(char *buffer)
{
    QString decodedString(buffer);

    if (decodedString.startsWith("PWRON")) {
        emit sigPower(true);
    } else
    if (decodedString.startsWith("PWROFF")) {
        emit sigPower(false);
    } else
    if (decodedString.startsWith("AGCON")) {
        emit sigAutomaticGainControl(true);
    } else
    if (decodedString.startsWith("AGCOFF")) {
        emit sigAutomaticGainControl(false);
    } else
    if (decodedString.startsWith("VSCON")) {
        emit sigVoiceControl(true);
    } else
    if (decodedString.startsWith("VSCOFF")) {
        emit sigVoiceControl(false);
    } else
    if (decodedString.startsWith("FILTER")) {
        emit sigFilter(0);
    } else
    if (decodedString.startsWith("FREQ")) {
        emit sigFrequency(0);
    } else
    if (decodedString.startsWith("IF")) {
        emit sigIFShift(128);
    } else
    if (decodedString.startsWith("MOD")) {
        emit sigModulation(0);
    } else
    if (decodedString.startsWith("NBON")) {
        emit sigNoiseBlanker(true);
    } else
    if (decodedString.startsWith("NBOFF")) {
        emit sigNoiseBlanker(true);
    } else
    {
        client->write("UNK");
        return;
    }

    client->write("ACK");

}

void CRemoteControl::sendData(QString value)
{
    if (connected)
        client->write(value.toLocal8Bit());
}
