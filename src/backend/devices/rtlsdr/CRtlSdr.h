#ifndef CRTLSDR_H
#define CRTLSDR_H

#include <QDebug>
#include <CBandScopeWorker.h>
#include "IDevice.h"
#include "CDemodulatorBase.h"
#include "pa_ringbuffer.h"
#include <pthread.h>
#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4200 )
#include <libusb.h>
#pragma warning( pop )
#else
#include <libusb-1.0/libusb.h>
#endif
#include <rtl-sdr.h>

#define RTLSDR_SAMPLE_RATE 1024000
#define DEFAULT_SAMPLE_RATE		24000
#define DEFAULT_BUF_LENGTH		(1 * 16384)
#define MAXIMUM_OVERSAMPLE		16
#define MAXIMUM_BUF_LENGTH		(MAXIMUM_OVERSAMPLE * DEFAULT_BUF_LENGTH)
#define AUTO_GAIN			-100
#define BUFFER_DUMP			4096
#define MAXIMUM_RATE			2400000

#define FREQUENCIES_LIMIT		1000


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
        void setAgcControl(bool state);
        void setDemodulator(uint mode);
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
        };
        struct demodule_state
        {
            int16_t *buf16;
            uint32_t buf_len;
        };
        void Initialize(struct dongle_state *s, demodule_state *d);
        struct dongle_state dongle;
        struct demodule_state demod;
        void Demodulate();
        void doBandscope();
        void setBandscope(CBandScopeWorker *bandscopeW);
        void setDemodulator(CDemodulatorBase *value);
    signals:
        void sigSampleRead(int16_t *buffer,int len);
    private:
        bool power;
        int device_count;
        CDemodulatorBase *demo;
        CBandScopeWorker *bandscopeW;
    private slots:
        void slotNewDataReceived(const QByteArray &dataReceived);
        void slotWatchdog();
};

#endif // CRTLSDR_H
