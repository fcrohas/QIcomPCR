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

      typedef struct datastream_t {
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
      void sigInitialize();

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

};

#endif // CREMOTECONTROL_H
