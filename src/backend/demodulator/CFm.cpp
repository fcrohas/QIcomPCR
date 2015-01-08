#include "CFm.h"

CFm::CFm(QObject *parent, Mode mode) :
    IDemodulator(parent)
{
}

void CFm::slotSamplesRead(int *buffer, int len) {

}
