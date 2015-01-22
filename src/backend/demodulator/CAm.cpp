#include "CAm.h"

CAm::CAm(QObject *parent, Mode mode) :
    CDemodulatorBase(parent),
    filter(NULL),
    filterIQ(NULL)
{
    // Build Bandpass filter
    winfunc = new CWindowFunc(this);
    winfunc->init(64);
    winfunc->hamming();
    // Low pass IQ FIR filter
    filterIQ = new CFIR<int16_t>();
    filterIQ->setOrder(32);
    filterIQ->setWindow(winfunc->getWindow());
    // Set FIR filter
    filter = new CFIR<int16_t>();
    filter->setOrder(64);
    filter->setWindow(winfunc->getWindow());
    this->mode = mode;
}

void CAm::doWork() {
    update.lock();
    // initalize
    int i, pcm;
    avgI = 0;
    avgQ = 0;
    // signal power
    int specSqrSum = 0;
    int sigSqrSum = 0;
    // First downsample 256000 for 1024000 rate on rtlsdr
    len = CDemodulatorBase::downsample(buffer,len, decimation);
    // Low pass filter IQ
    filterIQ->applyIQ(buffer,len);
    // Average DC
    averageIQ(buffer,len,&avgI, &avgQ);
    // signal sum
    int sigSum = 0;
    // Do demodulation
    for (i = 0; i < len; i += 2) {
        // hypot uses floats but won't overflow
        int iv = buffer[i] - avgI;
        int qv = buffer[i+1] - avgQ;
        pcm = iv*iv + qv*qv;
        // Power signal
        specSqrSum += buffer[i] * buffer[i] + buffer[i+1] * buffer[i+1];
        sigSqrSum += pcm;
        // Output stereo buffer only
        buffer[i/2] = (int16_t)sqrt(pcm);
        sigSum += buffer[i/2];
    }
    // update snr
    snr = sigSqrSum / specSqrSum;
    // half len channel
    len = len / 2;
    // average
    int halfPoint = sigSum / len;
    for (int i = 0; i < len; ++i) {
        buffer[i] = (buffer[i] - halfPoint) / halfPoint;
    }
    // apply filter
    filter->apply(buffer, len);
    // Apply audio filter
    len = CDemodulatorBase::resample(buffer,len,intfreq);
    CDemodulatorBase::processSound(buffer,len);
    update.unlock();
}

void CAm::slotSetFilter(uint frequency) {
    CDemodulatorBase::slotSetFilter(frequency);
    // audio filter
    if (filter != NULL) {
        filter->setSampleRate(22050);
        filter->lowpass(frequency);
    }
    // IQ filter
    if (filterIQ != NULL) {
        filterIQ->setSampleRate(intfreq);
        filterIQ->lowpass(frequency);
    }
}

QString CAm::getName() {
    return QString("CAm");
}

void CAm::averageIQ(int16_t *buffer,int len, int *avgI, int *avgQ) {
    int isum = 0;
    int qsum = 0;
    for (int i=0; i< len; i++) {
        isum += buffer[i];
        qsum += buffer[i+1];
    }
    *avgI = isum / len;
    *avgQ = qsum / len;
}
