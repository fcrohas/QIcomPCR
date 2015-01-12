#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent, Mode mode) :
    QObject(parent),
    sound(NULL),
    now_r(0),
    now_j(0),
    prev_index(0)
{

}

void IDemodulator::slotSetFilter(uint frequency) {
    downSampleFactor = 1024000/frequency;
}

void IDemodulator::setSoundDevice(ISound *device) {
    qDebug() << "Sound device is set to demodulator";
    sound = device;
}

void IDemodulator::doWork() {
}

void IDemodulator::setData(int16_t *buffer, int len) {
    this->buffer = buffer;
    this->len = len;
}

void IDemodulator::processSound(int16_t *buffer, int len) {
    if (sound != NULL)
        sound->Play(buffer,len);
}

void IDemodulator::downsample(int16_t *buffer, int len, int factor)
/* simple square window FIR */
{
    if (factor == 0) factor = downSampleFactor;
    int i=0, i2=0;
    while (i < len) {
        now_r += buffer[i];
        now_j += buffer[i+1];
        i += 2;
        prev_index++;
        if (prev_index < factor) {
            continue;
        }
        buffer[i2]   = now_r; // * d->output_scale;
        buffer[i2+1] = now_j; // * d->output_scale;
        prev_index = 0;
        now_r = 0;
        now_j = 0;
        i2 += 2;
    }
    len = i2;
}
