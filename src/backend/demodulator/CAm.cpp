#include "CAm.h"

CAm::CAm(QObject *parent, Mode mode) :
    IDemodulator(parent)
{
    // Build Bandpass filter
    winfunc = new CWindowFunc(this);
    winfunc->init(65);
    winfunc->hamming();
    // Set FIR filter
    filter = new CFIR<int>();
    filter->setOrder(64);
    filter->setWindow(winfunc->getWindow());
    filter->setSampleRate(22050);
    filter->lowpass(2800);

}

void CAm::slotSamplesRead(int *buffer, int len) {
    int i, pcm;
    for (i = 0; i < len; i += 2) {
        // hypot uses floats but won't overflow
        //r[i/2] = (int16_t)hypot(lp[i], lp[i+1]);
        pcm = buffer[i] * buffer[i];
        pcm += buffer[i+1] * buffer[i+1];
        buffer[i/2] = (int)sqrt(pcm) * 1;
    }
    // so new len is
    len = len/2;
    filter->apply(buffer,len);
}

void CAm::slotSetFilter(uint frequency) {
    filter->lowpass(frequency);
}
