#include "CDemodulatorBase.h"

CDemodulatorBase::CDemodulatorBase(QObject *parent, Mode mode) :
    QObject(parent),
    sound(NULL),
    now_r(0),
    now_j(0),
    prev_index(0),
    outputbufferf(NULL),
    inputbufferf(NULL),
    filterfreq(230000),
    samplerate(1024000),
    decimation(4),
    intfreq(256000),
    mode(eWFM),
    len(0)
{
    //qDebug() << "IDemodulator constructor...\r\n";
    // save mode
    this->mode = mode;
    // Build resample converter
    int error = 0;
    converter =  src_new( SRC_LINEAR , 1, &error);
    if (error != 0) {
        qDebug() << " Error creating sample rate converter : " << src_strerror(error);
    }
    slotSetFilter(filterfreq);
    //qDebug() << "IDemodulator constructor\r\n";
}

CDemodulatorBase::~CDemodulatorBase() {
    // Destroy sample converter
    if (converter != NULL)
        src_delete(converter);
}

void CDemodulatorBase::slotSetFilter(uint frequency) {
    filterfreq = frequency;
    //qDebug() << "decimation Factor=" << decimation << " samplerate=" << samplerate <<" filterfreq=" << filterfreq <<"\r\n";
}

void CDemodulatorBase::setSoundDevice(ISound *device) {
    qDebug() << "Sound device is set to demodulator";
    sound = device;
}

void CDemodulatorBase::doWork() {
}

void CDemodulatorBase::setData(int16_t *buffer, int len) {
    this->buffer = buffer;
    this->len = len;
}

void CDemodulatorBase::processSound(int16_t *buffer, int len) {
    if (sound != NULL)
        sound->Play(buffer,len);
}

int CDemodulatorBase::downsample(int16_t *buffer, int len, int factor)
/* simple square window FIR */
{
    if (factor == 0) factor = decimation;
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
    // Windowing sample

    return i2;
}

int CDemodulatorBase::resample(int16_t *buffer, int len, int samplerate) {
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
    dataconv.output_frames = len;
    dataconv.src_ratio = ratio;
    dataconv.end_of_input = 0;
    // Do convert
    int ret = src_process(converter, &dataconv);
    if (ret != 0) {
        qDebug() << "input frames("<< len<<") used is " << dataconv.input_frames_used << "frames gen is " << dataconv.output_frames_gen <<" error="<< src_strerror (ret) <<"\r\n";
    }
    // Convert back to int16_t aka short
    src_float_to_short_array(outputbufferf,buffer,dataconv.output_frames_gen);
    return dataconv.output_frames_gen;
}

int CDemodulatorBase::low_pass_real(int16_t *buffer, int len)
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

int CDemodulatorBase::rms(int step)
/* largely lifted from rtl_power */
{
    int i;
    long p, t, s;
    double dc, err;

    p = t = 0L;
    for (i=0; i<len; i+=step) {
        s = (long)buffer[i];
        t += s;
        p += s * s;
    }
    /* correct for dc offset in squares */
    dc = (double)(t*step) / (double)len;
    err = t * 2 * dc - dc * dc * len;

    return (int)sqrt((p-err) / len);
}

int CDemodulatorBase::mad(int step)
/* mean average deviation */
{
    int i=0, sum=0, ave=0;
    if (len == 0)
        {return 0;}
    for (i=0; i<len; i+=step) {
        sum += buffer[i];
    }
    ave = sum / (len * step);
    sum = 0;
    for (i=0; i<len; i+=step) {
        sum += abs(buffer[i] - ave);
    }
    int result = sum / (len / step);
    // limit result between 0 and 255;
    result = result * 255 / 32768 + 127;
    return result;
}

void CDemodulatorBase::setSampleRate(uint frequency) {
    // sample rate of input device
    samplerate = frequency;
    // Want 250K samplerate for work
    decimation = samplerate / 250000;
    // first intermediate frequency
    intfreq = samplerate / decimation;

}

QString CDemodulatorBase::getName() {
    return QString("CDemodulatorBase");
}
