#ifndef CREMOTECONTROL_H
#define CREMOTECONTROL_H

#include <QtNetwork>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

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

    private:
      QTcpServer server;
      QTcpSocket* client;
      bool connected;

    signals:
      void command(QString &value);
};

#endif // CREMOTECONTROL_H
