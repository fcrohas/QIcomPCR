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

      struct datastream_t {
          datastream_t() : refreshRate(0.0), enabled(false), low(0), high(0) {}
          double refreshRate;
          bool enabled;
          int low;
          int high;
      };
      datastream_t datastream;

    signals:
      void command(QString &value);

      // Init device
      void sigInitialize(bool);

      // Power command
      void sigPower(bool value);

      // Set frequency
      void sigFrequency(uint value);

      // Set frequency modulation
      void sigModulation(uint value);

      // set Automatic Gain Control Mode
      void sigAutomaticGainControl(bool value);

      // set Noise Blanker
      void sigNoiseBlanker(bool value);

      // Set Antenna
      void sigRadio(uint value);

      // Set Filter
      void sigFilter(uint value);

      // Set IFShift
      void sigIFShift(uint value);

      // set Voice Control
      void sigVoiceControl(bool value);

      // set Radio mode
      void sigRadioMode(uint value);

      // set Sound Output
      void sigSoundVolume(uint value);
      void sigSoundMute(bool value);

      // set Squelch
      void sigSquelch(uint value);

      // Frequency
      void sigSelectFrequency(double value);
      void sigSelectBandwidth(double value);

      // Frequency
      void sigChannel(int value);
      void sigDecoder(int value);

};

#endif // CREMOTECONTROL_H
