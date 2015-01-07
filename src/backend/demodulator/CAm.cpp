#include "CAm.h"

CAm::CAm(QObject *parent, Mode mode) :
    IDemodulator(parent)
{
}

void CAm::slotSamplesRead(int16_t *buffer, int len) {
    int32_t i, pcm;
    for (i = 0; i < len; i += 2) {
        // hypot uses floats but won't overflow
        //r[i/2] = (int16_t)hypot(lp[i], lp[i+1]);
        pcm = buffer[i] * buffer[i];
        pcm += buffer[i+1] * buffer[i+1];
        //lp[i/2] = (int16_t)sqrt(pcm) * fm->output_scale;
    }
    //fm->lp_len = fm->lp_len / 2;
}
