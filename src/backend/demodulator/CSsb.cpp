#include "CSsb.h"

CSsb::CSsb(QObject *parent,Mode mode) :
    IDemodulator(parent)
{
}

void CSsb::slotSamplesRead(int *buffer, int len) {

}
