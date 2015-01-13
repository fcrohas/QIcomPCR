#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent, Mode mode) :
    QObject(parent),
    sound(NULL),
    now_r(0),
    now_j(0),
    prev_index(0),
    outputbufferf(NULL),
    inputbufferf(NULL),
    filterfreq(230000),
    update(false)
{
    downSampleFactor = 1024000/filterfreq;
    this->mode = mode;
}

void IDemodulator::slotSetFilter(uint frequency) {
    filterfreq = frequency;
    downSampleFactor = 1024000/filterfreq;
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

int IDemodulator::downsample(int16_t *buffer, int len, int factor)
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
    return len;
}

int IDemodulator::resample(int16_t *buffer, int len, int samplerate) {
    if (inputbufferf == NULL)
        inputbufferf = new float[len];
    if (outputbufferf == NULL)
        outputbufferf = new float[len];
    // Resample ratio
    double ratio = SAMPLERATE / samplerate;
    src_short_to_float_array(buffer,inputbufferf,len);
    // fill converter parameters
    dataconv.data_in = inputbufferf;
    dataconv.input_frames = len/2;
    dataconv.data_out = outputbufferf;
    dataconv.output_frames = len/2;
    dataconv.src_ratio = ratio;
    dataconv.end_of_input = 0;
    // Do convert
    int ret = src_simple(&dataconv,SRC_LINEAR,2);
    if (ret != 0) {
        qDebug() << "input frames("<< len/2<<") used is " << dataconv.input_frames_used << "frames gen is " << dataconv.output_frames_gen <<" error="<< src_strerror (ret) <<"\r\n";
    }
    // Convert back to int16_t aka short
    src_float_to_short_array(outputbufferf,buffer,dataconv.output_frames_gen);
    return dataconv.output_frames_gen;
}
