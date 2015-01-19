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
#ifndef CREMOTECONTROL_H
#define CREMOTECONTROL_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <CCommand.h>

class CRemoteControl: public QObject
{
    Q_OBJECT
    public:
      CRemoteControl(QObject * parent = 0);
      ~CRemoteControl();
      void decode(char *buffer);

    public slots:
      void acceptConnection();
      void startRead();
      void sendData(QString value);
      void controledRate(double *xval, double *yval, int size);

    private:
      QTcpServer server;
      QTcpSocket* client;
      QTime time;
      bool connected;
      double *avg;
      CCommand::bandscope_t bandscope;
      CCommand::radio_t radio;
      CDecoder::decoder_t decoder;

      struct datastream_t {
          datastream_t() : refreshRate(0.0), enabled(false), low(0), high(0) {}
          double refreshRate;
          bool enabled;
          int low;
          int high;
      };
      datastream_t datastream;
      bool isRadioCommand(QString decodedString);
      bool isDecoderCommand(QString decodedString);
      bool isBandscopeCommand(QString decodedString);

    signals:
      void command(QString &value);

      // Radio
      void sigRadio(CCommand::radio_t radio);

      // Init device
      void sigInitialize(bool);

      // Bandscope mode
      void sigBandScope(CCommand::bandscope_t bandscope);

      // set Sound Output
      void sigSoundVolume(uint value);
      void sigSoundMute(bool value);

      // Decoder
      void sigDecoder(CDecoder::decoder_t decoder);
};

#endif // CREMOTECONTROL_H
