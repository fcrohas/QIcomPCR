#ifndef CRTLSDR_H
#define CRTLSDR_H

#include <QDebug>
#include "IDevice.h"
#include <pthread.h>
#include <libusb-1.0/libusb.h>
#include <rtl-sdr.h>

class CRtlSdr : public IDevice
{
    Q_OBJECT
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
        void setFrequency(uint freq);
        uint getFrequency();
    private:
        struct dongle_state
        {
            int      exit_flag;
            pthread_t thread;
            rtlsdr_dev_t *dev;
            int      dev_index;
            uint32_t freq;
            uint32_t rate;
            int      gain;
            int16_t  *buf16;
            uint32_t buf_len;
            int      ppm_error;
            int      offset_tuning;
            int      direct_sampling;
            int      mute;
            int      pre_rotate;
            struct demod_state *targets[2];
        };
        struct dongle_state dongle;

    private slots:
        void slotNewDataReceived(const QByteArray &dataReceived);
        void slotWatchdog();

};

#endif // CRTLSDR_H
