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
    if (isRadioCommand(decodedString)) {
        emit sigRadio(radio);
    } else
    if (isBandscopeCommand(decodedString)) {
        emit sigBandScope(bandscope);
    } else
    if (isDecoderCommand(decodedString)) {
        emit sigDecoder(decoder);
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

bool CRemoteControl::isRadioCommand(QString decodedString) {
    bool found = true;
    if (decodedString.startsWith("PWRON")) {
        radio.power = true;
    } else
    if (decodedString.startsWith("PWROFF")) {
        radio.power = false;
    } else
    if (decodedString.startsWith("RADIO")) {
        QString antenna= decodedString.replace("RADIO","");
        radio.antenna = antenna.toInt();
        emit sigRadio(radio);
    } else
    if (decodedString.startsWith("AGCON")) {
        radio.agc = true;
    } else
    if (decodedString.startsWith("AGCOFF")) {
        radio.agc = false;
    } else
    if (decodedString.startsWith("VSCON")) {
        radio.vsc = true;
    } else
    if (decodedString.startsWith("VSCOFF")) {
        radio.vsc = false;
    } else
    if (decodedString.startsWith("FILTER")) {
        QString filter = decodedString.replace("FILTER","");
        if (filter == "230000") radio.filter = CCommand::e230k;
        if (filter == "50000")  radio.filter = CCommand::e50k;
        if (filter == "15000")  radio.filter = CCommand::e15k;
        if (filter == "6000")   radio.filter = CCommand::e6k;
        if (filter == "2800")  radio.filter = CCommand::e28k;
    } else
    if (decodedString.startsWith("FREQ")) {
        QString frequency = decodedString.replace("FREQ","");
        radio.frequency = frequency.toUInt();
    } else
    if (decodedString.startsWith("IF")) {
        QString shift = decodedString.replace("IF","");
        radio.ifshift = shift.toUInt();
    } else
    if (decodedString.startsWith("SQUELCH")) {
        QString sq = decodedString.replace("SQUELCH","");
        radio.squelch = sq.toUInt();
    } else
    if (decodedString.startsWith("MOD")) {
        QString modulation = decodedString.replace("MOD","");
        if (modulation == "WFM") radio.modulation = CCommand::eWFM;
        if (modulation == "FM")  radio.modulation = CCommand::eFM;
        if (modulation == "AM")  radio.modulation = CCommand::eAM;
        if (modulation == "LSB") radio.modulation = CCommand::eLSB;
        if (modulation == "USB") radio.modulation = CCommand::eUSB;
        if (modulation == "CW")  radio.modulation = CCommand::eCW;
    } else
    if (decodedString.startsWith("NBON")) {
        radio.nb = true;
    } else
    if (decodedString.startsWith("NBOFF")) {
        radio.nb = false;
    } else
        found = false;
    return found;
}

bool CRemoteControl::isBandscopeCommand(QString decodedString) {
    bool found = true;
    /* Bandscope */
    if (decodedString.startsWith("BDSON")) {
        bandscope.power = true;
        emit sigBandScope(bandscope);
    } else
    if (decodedString.startsWith("BDSOFF")) {
        bandscope.power = false;
        emit sigBandScope(bandscope);
    } else
    if (decodedString.startsWith("BDSW")) {
        QString width= decodedString.replace("BDSW","");
        bandscope.width = width.toInt();
        emit sigBandScope(bandscope);
    } else
    if (decodedString.startsWith("BDSS")) {
        QString step= decodedString.replace("BDSS","");
        bandscope.step = step.toInt();
        emit sigBandScope(bandscope);
    } else
        found = false;
    return found;
}

bool CRemoteControl::isDecoderCommand(QString decodedString) {
    bool found = true;
    if (decodedString.startsWith("CHAN")) {
        QString value= decodedString.replace("CHAN","");
        decoder.channel = value.toInt();
        emit sigDecoder(decoder);
    } else
    if (decodedString.startsWith("DEC")) {
        QString value= decodedString.replace("DEC","");
        decoder.type = value.toInt();
        emit sigDecoder(decoder);
    } else
    if (decodedString.startsWith("SFREQ")) {
        QString frequency = decodedString.replace("SFREQ","");
        decoder.frequency = frequency.toDouble();
        emit sigDecoder(decoder);
    } else
    if (decodedString.startsWith("SFRQW")) {
        QString width = decodedString.replace("SFRQW","");
        decoder.bandwidth = width.toDouble();
        emit sigDecoder(decoder);
    } else
        found = false;
    return found;
}
