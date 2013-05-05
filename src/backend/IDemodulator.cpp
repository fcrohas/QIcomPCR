#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent) :
    QObject(parent)
{
}

uint IDemodulator::getChannel()
{
    return 0;
}

uint IDemodulator::getDataSize()
{
    return 0;
}

void IDemodulator::decode(int16_t *buffer, int size, int offset)
{

}

void IDemodulator::decode(uchar *buffer, int size, int offset)
{

}

uint IDemodulator::getBufferSize()
{
    return 0;
}

void IDemodulator::slotFrequency(double value)
{
    return;
}

void IDemodulator::slotBandwidth(int value)
{
    return;
}

void IDemodulator::setThreshold(int value)
{

}

void IDemodulator::setCorrelationLength(int value)
{

}
