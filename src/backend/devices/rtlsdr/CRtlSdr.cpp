#include "CRtlSdr.h"

extern "C" {

void rotate_90(unsigned char *buf, uint32_t len)
/* 90 rotation is 1+0j, 0+1j, -1+0j, 0-1j
   or [0, 1, -3, 2, -4, -5, 7, -6] */
{
    uint32_t i;
    unsigned char tmp;
    for (i=0; i<len; i+=8) {
        /* uint8_t negation = 255 - x */
        tmp = 255 - buf[i+3];
        buf[i+3] = buf[i+2];
        buf[i+2] = tmp;

        buf[i+4] = 255 - buf[i+4];
        buf[i+5] = 255 - buf[i+5];

        tmp = 255 - buf[i+6];
        buf[i+6] = buf[i+7];
        buf[i+7] = tmp;
    }
}

static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
    {
        int i;
        CRtlSdr *This = (CRtlSdr *)ctx;
        struct CRtlSdr::dongle_state *s = &This->dongle;
        if (!s) {
            return;
        }
        if (s->mute) {
            for (i=0; i<s->mute; i++) {
                buf[i] = 127;}
            s->mute = 0;
        }
        //rotate_90(buf,len);
        for (i=0; i<(int)len; i++) {
            s->buf16[i] = (int16_t)buf[i] - 127;}
        s->buf_len = len;
        This->Demodulate();
    }

    static void *dongle_thread_fn(void *arg)
    {
        CRtlSdr *s = (CRtlSdr*)arg;
        rtlsdr_read_async(s->dongle.dev, rtlsdr_callback, (void*)arg, 0, s->dongle.buf_len);
        return 0;
    }
}

CRtlSdr::CRtlSdr(QObject * parent)
    : IDevice("RTLSDR",parent),
      power(false)
{
    device_count = rtlsdr_get_device_count();
    if (device_count>0) {
        char vendor[256] = {0}, product[256] = {0}, serial[256] = {0};
        for (int i=0; i< device_count; i++) {
            if (rtlsdr_get_device_usb_strings(i, vendor, product, serial) == 0) {
                qDebug() << i << ":" << vendor << "," << product << " SN:"<< serial << "\n";
            } else {
                qDebug() << i << ":" << vendor << "Failed to query data";
            }
        }
    }
    log_t.dataCount = 0;
    log_t.dataReceive = 0;
    log_t.dataSent = 0;
    log_t.isConnected = false;
}

CRtlSdr::~CRtlSdr() {
    close();
}

void CRtlSdr::Initialize(struct dongle_state *s, struct demodule_state *d)
{
    // dongle state init
    s->rate = 1024000; //RTLSDR_SAMPLE_RATE;
    s->gain = 1; // tenths of a dB
    s->mute = 0;
    s->direct_sampling = 0;
    s->offset_tuning = 0;
    s->buf16 = new int16_t[MAXIMUM_BUF_LENGTH];
    // demodulator state init
    d->buf16 = new int16_t[MAXIMUM_BUF_LENGTH];
}

bool CRtlSdr::open() {
    if (device_count == 0) {
        qDebug() << "No supported device found to open rtlsdr device " << dongle.dev_index << "\n";
        power = false;
        return false;
    }
    Initialize(&dongle,&demod);
    dongle.dev_index = 0;

    if (dongle.dev_index < 0) {
        power = false;
        return false;
    }

    int r = rtlsdr_open(&dongle.dev, (uint32_t)dongle.dev_index);
    if (r < 0) {
        qDebug() << "Failed to open rtlsdr device " << dongle.dev_index << "\n";
        power = false;
        return false;
    }

    r = rtlsdr_set_tuner_gain_mode(dongle.dev,dongle.gain);
    if (r < 0) {
        qDebug() << "Failed to set gain to " << dongle.gain << "\n";
        power = false;
        return false;
    }

    r = rtlsdr_set_tuner_if_gain(dongle.dev,1,20);
    if (r < 0) {
        qDebug() << "Failed to set gain to " << dongle.gain << "\n";
        power = false;
        return false;
    }

    r = rtlsdr_reset_buffer(dongle.dev);
    if (r < 0) {
        qDebug() << "WARNING: Failed to reset buffers.\n";
    }

    r = rtlsdr_set_sample_rate(dongle.dev,dongle.rate);
    if (r < 0) {
        qDebug() << "Failed to set sample rate to " << dongle.rate << "\n";
        power = false;
        return false;
    }

    r = rtlsdr_set_freq_correction(dongle.dev,43);
    if (r < 0) {
        qDebug() << "Failed to set ppm rate to " << 43 << "\n";
        power = false;
        return false;
    }

    qDebug() << "Dongle sample rate " << rtlsdr_get_sample_rate(dongle.dev);

    // Build a data reader thead
    pthread_create(&dongle.thread, NULL, dongle_thread_fn, (void *)(this));
    power = true;
    log_t.isConnected = true;
    return true;
}

void CRtlSdr::write(QString &data) {

}

void CRtlSdr::close() {
    log_t.isConnected = false;
    // Stop thread
    if (dongle.dev != NULL)
        rtlsdr_cancel_async(dongle.dev);
    // Join on leave
#ifdef WIN32
    if (dongle.thread.p != 0)
#else
    if (dongle.thread != 0)
#endif
        pthread_join(dongle.thread, NULL);
    // Close rtl device
    int r = rtlsdr_close(dongle.dev);
    if (r < 0) {
        qDebug() << "Failed to close rtlsdr device " << dongle.dev_index << "\n";
    }
}


void CRtlSdr::slotNewDataReceived(const QByteArray &dataReceived) {

}

void CRtlSdr::slotWatchdog() {

}

void CRtlSdr::setFrequency(uint freq) {
    if (!power)
        return;
    int ret = rtlsdr_set_center_freq(dongle.dev, freq);
    if (ret<0)
        qDebug() << "Frequency not set to " << freq << "\n";
}

uint CRtlSdr::getFrequency() {
    if (!power)
        return 0;
    return rtlsdr_get_center_freq(dongle.dev);
}

void CRtlSdr::setAgcControl(bool state) {
    if (!power)
        return;
    int ret = rtlsdr_set_agc_mode(dongle.dev, state);
    if (ret<0)
        qDebug() << "AGC not set to " << state << "\n";
}

void CRtlSdr::Demodulate() {
    demo->update.lock();
    log_t.dataReceive += dongle.buf_len;
    demod.buf_len = dongle.buf_len;
    memcpy(demod.buf16, dongle.buf16, dongle.buf_len*2);
    demo->setData(demod.buf16,demod.buf_len);
    demo->update.unlock();
    QMetaObject::invokeMethod(demo, "doWork");
}

void CRtlSdr::setDemodulator(CDemodulatorBase *value) {
    demo = value;
    qDebug() << "Set demodulator " << demo->getName() <<"\r\n";
}
