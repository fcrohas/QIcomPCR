#include "CAm.h"

CAm::CAm(QObject *parent, Mode mode) :
    IDemodulator(parent)
{
    // Build Bandpass filter
    winfunc = new CWindowFunc(this);
    winfunc->init(64);
    winfunc->hamming();
    // Set FIR filter
    filter = new CFIR<int16_t>();
    filter->setOrder(64);
    filter->setWindow(winfunc->getWindow());
    this->mode = mode;
}

void CAm::doWork() {
    update.lock();
    int i, pcm;
    // First downsample
    len = IDemodulator::downsample(buffer,len, decimation);
    // Do demodulation
    for (i = 0; i < len; i += 2) {
        // hypot uses floats but won't overflow
        //r[i/2] = (int16_t)hypot(lp[i], lp[i+1]);
        pcm = buffer[i] * buffer[i];
        pcm += buffer[i+1] * buffer[i+1];
        // Output stereo buffer only
        buffer[i/2] = (int16_t)sqrt(pcm);
    }
    // half len channel
    len = len / 2;
    // apply filter
    filter->apply(buffer, len);
    // Apply audio filter
    len = IDemodulator::resample(buffer,len,intfreq);
    IDemodulator::processSound(buffer,len);
    update.unlock();
}

void CAm::slotSetFilter(uint frequency) {
    IDemodulator::slotSetFilter(frequency);
    if (filter != NULL) {
        filter->setSampleRate(22050);
        filter->lowpass(frequency);
        //
        filter->convert();
    }
}
