#include "CRtlSdr.h"

CRtlSdr::CRtlSdr(QObject * parent)
    : IDevice("RTLSDR",parent)
{
}

CRtlSdr::~CRtlSdr() {

}

bool CRtlSdr::open() {
    dongle.dev_index = 0;

    if (dongle.dev_index < 0) {
        return false;
    }

    int r = rtlsdr_open(&dongle.dev, (uint32_t)dongle.dev_index);
    if (r < 0) {
        qDebug() << "Failed to open rtlsdr device " << dongle.dev_index << "\n";
        return false;
    }
    return true;
}

void CRtlSdr::write(QString &data) {
}

void CRtlSdr::close() {
    int r = rtlsdr_close(dongle.dev);
    if (r < 0) {
        qDebug() << "Failed to close rtlsdr device " << dongle.dev_index << "\n";
    }
}


void CRtlSdr::slotNewDataReceived(const QByteArray &dataReceived) {

}

void CRtlSdr::slotWatchdog() {

}
