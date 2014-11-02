#ifndef CRTLSDR_H
#define CRTLSDR_H

#include "IDevice.h"

class CRtlSdr : public IDevice
{
    Q_OBJECT;
    public:
        CRtlSdr(QObject * parent);
        virtual ~CRtlSdr();
        void write(QString &data);
        bool open();
        struct log {
            bool isConnected;
            int dataCount;
            int dataSent;
            int dataReceive;

        } log_t;
        void close();

    private slots:
        void slotNewDataReceived(const QByteArray &dataReceived);
        void slotWatchdog();

};

#endif // CRTLSDR_H
