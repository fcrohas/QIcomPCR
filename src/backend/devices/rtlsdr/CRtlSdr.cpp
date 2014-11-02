#include "CRtlSdr.h"

CRtlSdr::CRtlSdr(QObject * parent)
    : IDevice("ICOMPCR",parent)
{
}

CRtlSdr::~CRtlSdr() {

}

bool CRtlSdr::open() {
    return false;
}

void CRtlSdr::write(QString &data) {

}

void CRtlSdr::close() {

}


void CRtlSdr::slotNewDataReceived(const QByteArray &dataReceived) {

}

void CRtlSdr::slotWatchdog() {

}
