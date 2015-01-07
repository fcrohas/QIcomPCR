#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent, Mode mode) :
    QObject(parent)
{

}

void IDemodulator::slotSamplesRead(int16_t *buffer, int len) {

}
