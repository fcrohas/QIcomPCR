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
    filter->setSampleRate(22050);
    filter->lowpass(11000);

}

void CAm::doWork() {
    int i, pcm;
    // First downsample
    IDemodulator::downsample(buffer,len);
    // Do demodulation
    for (i = 0; i < len; i += 2) {
        // hypot uses floats but won't overflow
        //r[i/2] = (int16_t)hypot(lp[i], lp[i+1]);
        pcm = buffer[i] * buffer[i];
        pcm += buffer[i+1] * buffer[i+1];
        // Output stereo buffer only
        buffer[i/2] = (int)sqrt(pcm) * 1;
    }
    // so new len is
    len = len/2;
    // Apply audio filter
    //filter->apply(buffer,len);
    IDemodulator::processSound(buffer,len);
}

void CAm::slotSetFilter(uint frequency) {
    qDebug() << "Change frequency filter to " << frequency << " Hz";
    filter = frequency;
    IDemodulator.slotSetFilter(frequency);
    //filter->lowpass(frequency);
}
