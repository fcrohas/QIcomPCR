#include "CRtlSdr.h"

extern "C" {

    static void rtlsdr_callback(unsigned char *buf, uint32_t len, void *ctx)
    {
        int i;
        CRtlSdr *This = (CRtlSdr *)ctx;
        struct CRtlSdr::dongle_state *s = &This->dongle;
        This->Demodulate();
        if (!s) {
            return;
        }
        if (s->mute) {
            for (i=0; i<s->mute; i++) {
                buf[i] = 127;}
            s->mute = 0;
        }
        for (i=0; i<(int)len; i++) {
            s->buf16[i] = (int16_t)buf[i] - 127;}
    }

    static void *dongle_thread_fn(void *arg)
    {
        struct CRtlSdr::dongle_state *s = (CRtlSdr::dongle_state*)arg;
        rtlsdr_read_async(s->dev, rtlsdr_callback, s, 0, s->buf_len);
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
}

CRtlSdr::~CRtlSdr() {

}

void CRtlSdr::Initialize(struct dongle_state *s)
{
    s->rate = DEFAULT_SAMPLE_RATE;
    s->gain = AUTO_GAIN; // tenths of a dB
    s->mute = 0;
    s->direct_sampling = 0;
    s->offset_tuning = 0;
}

bool CRtlSdr::open() {
    if (device_count == 0) {
        qDebug() << "No supported device found to open rtlsdr device " << dongle.dev_index << "\n";
        power = false;
        return false;
    }
    Initialize(&dongle);
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
    r = rtlsdr_reset_buffer(dongle.dev);
    if (r < 0) {
        qDebug() << "WARNING: Failed to reset buffers.\n";
    }
    // Build a data reader thead
    pthread_create(&dongle.thread, NULL, dongle_thread_fn, (void *)(this));
    power = true;
    return true;
}

void CRtlSdr::write(QString &data) {

}

void CRtlSdr::close() {
    // Stop thread
    rtlsdr_cancel_async(dongle.dev);
    // Join on leave
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
    qDebug() << "Dongle buffer len=" << dongle.buf_len;
}
