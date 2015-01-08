#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent, Mode mode) :
    QObject(parent)
{

}

void IDemodulator::slotSamplesRead(int *buffer, int len) {

}

void IDemodulator::slotSetFilter(uint frequency) {

}
