#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent, Mode mode) :
    QObject(parent),
    sound(NULL),
    now_r(0),
    now_j(0),
    prev_index(0),
    outputbufferf(NULL),
    inputbufferf(NULL),
    filterfreq(230000)
{
    downSampleFactor = 1024000/(filterfreq); // Sample rate is twice filter size
    this->mode = mode;
    // Build resample converter
    int error = 0;
    converter =  src_new( SRC_LINEAR , 1, &error);
    if (error != 0) {
        qDebug() << " Error creating sample rate converter : " << src_strerror(error);
    }
}

IDemodulator::~IDemodulator() {
    if (converter != NULL)
        src_delete(converter);
}

void IDemodulator::slotSetFilter(uint frequency) {
    filterfreq = frequency;
    downSampleFactor = 1024000/(filterfreq); // Sample rate is twice filter size
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
    return i2;
}

int IDemodulator::resample(int16_t *buffer, int len, int samplerate) {
    if (inputbufferf == NULL)
        inputbufferf = new float[len];
    if (outputbufferf == NULL)
        outputbufferf = new float[len];

    // Resample ratio
    double ratio = SAMPLERATE*1.0 / samplerate*1.0;
    src_short_to_float_array(buffer,inputbufferf,len);
    // fill converter parameters
    dataconv.input_frames = len;
    dataconv.data_in = inputbufferf;
    dataconv.data_out = outputbufferf;
    dataconv.output_frames = 4096;
    dataconv.src_ratio = ratio;
    dataconv.end_of_input = 0;
    // Do convert
    int ret = src_process(converter, &dataconv);
    if (ret == 0) {
        qDebug() << "input frames("<< len<<") used is " << dataconv.input_frames_used << "frames gen is " << dataconv.output_frames_gen <<" error="<< src_strerror (ret) <<"\r\n";
    }
    // Convert back to int16_t aka short
    src_float_to_short_array(outputbufferf,buffer,dataconv.output_frames_gen);
    return dataconv.output_frames_gen;
}

int IDemodulator::low_pass_real(int16_t *buffer, int len)
/* simple square window FIR */
// add support for upsampling?
{
    int i=0, i2=0;
    int fast = filterfreq;
    int slow = 22050;
    while (i < len) {
        now_r += buffer[i];
        i+=1;
        prev_index += slow;
        if (prev_index < fast) {
            continue;
        }
        buffer[i2] = (int16_t)(now_r / (fast/slow));
        //buffer[i2+1] = (int16_t)(now_r / (fast/slow));
        prev_index -= fast;
        now_r = 0;
        i2 += 1;
    }
    return i2;
}
