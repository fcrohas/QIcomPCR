#include "CRemoteControl.h"

#include <iostream>

CRemoteControl::CRemoteControl(QObject* parent): QObject(parent)
  , avg(NULL)
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

    if (decodedString.startsWith("INIT")) {
        emit sigInitialize(true);
    } else
    if (decodedString.startsWith("PWRON")) {
        emit sigInitialize(true);
        //emit sigPower(true);
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
        QString filter = decodedString.replace("FILTER","");
        if (filter == "230000") emit sigFilter(CCommand::e230k);
        if (filter == "50000")  emit sigFilter(CCommand::e50k);
        if (filter == "15000")  emit sigFilter(CCommand::e15k);
        if (filter == "6000")   emit sigFilter(CCommand::e6k);
        if (filter == "2800")  emit sigFilter(CCommand::e28k);
    } else
    if (decodedString.startsWith("FREQ")) {
        QString frequency = decodedString.replace("FREQ","");
        //qDebug() << "frequency remote " << frequency.toUInt();
        emit sigFrequency(frequency.toUInt());
    } else
    if (decodedString.startsWith("IF")) {
        QString shift = decodedString.replace("IF","");
        emit sigIFShift(shift.toUInt());
    } else
    if (decodedString.startsWith("MOD")) {
        QString modulation = decodedString.replace("MOD","");
        if (modulation == "WFM") emit sigModulation(CCommand::eWFM);
        if (modulation == "FM")  emit sigModulation(CCommand::eFM);
        if (modulation == "AM")  emit sigModulation(CCommand::eAM);
        if (modulation == "LSB") emit sigModulation(CCommand::eLSB);
        if (modulation == "USB") emit sigModulation(CCommand::eUSB);
        if (modulation == "CW")  emit sigModulation(CCommand::eCW);
    } else
    if (decodedString.startsWith("SFREQ")) {
        QString frequency = decodedString.replace("SFREQ","");
        emit sigSelectFrequency(frequency.toDouble());
    } else
    if (decodedString.startsWith("SFRQW")) {
        QString width = decodedString.replace("SFRQW","");
        emit sigSelectBandwidth(width.toDouble());
    } else
    if (decodedString.startsWith("NBON")) {
        emit sigNoiseBlanker(true);
    } else
    if (decodedString.startsWith("NBOFF")) {
        emit sigNoiseBlanker(true);
    } else
    if (decodedString.startsWith("RADIO")) {
        QString radio= decodedString.replace("RADIO","");
        emit sigRadio(radio.toInt());
    } else
    if (decodedString.startsWith("CHAN")) {
        QString value= decodedString.replace("CHAN","");
        emit sigChannel(value.toInt());
    } else
    if (decodedString.startsWith("DEC")) {
        QString value= decodedString.replace("DEC","");
        emit sigDecoder(value.toInt());
    } else
    if (decodedString.startsWith("WT")) {
        QString params = decodedString.replace("WT","");
        if (params == "OFF") {
            datastream.enabled = false;
        }
        else {
            bool ok = false;
            datastream.enabled = true;
            datastream.refreshRate = params.toDouble(&ok) * 1000;
            //qDebug() << "receive WT on with refresh " << datastream.refreshRate << " ms";
            if (!ok) {
                datastream.enabled = false;
            } else time.start();

        }
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

void CRemoteControl::controledRate(double *xval, double *yval, int size)
{
    if (avg == NULL) {
        avg = new double[size];
        for (int i=0; i < size; i++ ) {
            avg[i] = 0.0;
        }
    }

    if ((datastream.enabled) && (time.elapsed()>datastream.refreshRate)) {
        // Maybe normalize data between datastream_t.low and high
        QString data;
        for (int i=0; i< size; i++) {
            data += QString("%1").arg((int)avg[i], 2, 16, QChar('0'));
        }
        data = data.prepend("@WT%1").arg(size, 4, 16, QChar('0'));
        data = data.append('@');
        //qDebug() << "Scope " << data;
        sendData(data);
        // Restart timer
        time.restart();
    } else {
        // Average value in time
        // this gave a smoother waterfall
        for (int i=0; i < size; i++ ) {
            avg[i] = (yval[i] + avg[i]) / 2.0;
        }
    }
}
