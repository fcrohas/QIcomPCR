#include "CFm.h"

CFm::CFm(QObject *parent, Mode mode) :
    CDemodulatorBase(parent),
    avg(0),
    filter(NULL),
    deemph_a(1)
{
    //qDebug() << "CFM constructor...\r\n";
    // Build Window blackman
    winfunc = new CWindowFunc(this);
    winfunc->init(32);
    winfunc->blackmanharris();
    // Set FIR filter
    // Build Bandpass filter
    filter = new CFIR<int16_t>();
    filter->setOrder(32);
    filter->setWindow(winfunc->getWindow());
    this->mode = mode;
    slotSetFilter(filterfreq);
    //qDebug() << "CFM constructor Mode="<< mode << "\r\n";

}

void CFm::doWork() {
    update.lock();
    len = CDemodulatorBase::downsample(buffer,len,decimation);
    int16_t pr = pre_real;
    int16_t pj = pre_img;
    int pcm = 0;
    // demodulate FM
    for (int i = 0; i < len; i += 2) {
        if (mode == eWFM)
            pcm = polar_disc_fast(buffer[i], buffer[i+1], pr, pj);
        else
            pcm = polar_discriminant(buffer[i], buffer[i+1], pr, pj);
        pr = buffer[i];
        pj = buffer[i+1];
        buffer[i/2] = (int16_t)pcm;
    }
    // Update new length for one channel
    len = len/2;
    // Deemphasis filter if WBFM mode
    if (mode == eWFM) {
        deemph_filter(buffer,len);
    }
    // Apply audio filter 30 Hz - 11kHz
    filter->apply(buffer,len);
    // resample for sound output
    len = CDemodulatorBase::resample(buffer,len,intfreq);
    // send sound to queue
    CDemodulatorBase::processSound(buffer,len);
    update.unlock();
}

int CFm::esbensen(int ar, int aj, int br, int bj)
/*
  input signal: s(t) = a*exp(-i*w*t+p)
  a = amplitude, w = angular freq, p = phase difference
  solve w
  s' = -i(w)*a*exp(-i*w*t+p)
  s'*conj(s) = -i*w*a*a
  s'*conj(s) / |s|^2 = -i*w
*/
{
    int cj, dr, dj;
    int scaled_pi = 2608; /* 1<<14 / (2*pi) */
    dr = (br - ar) * 2;
    dj = (bj - aj) * 2;
    cj = bj*dr - br*dj; /* imag(ds*conj(s)) */
    return (scaled_pi * cj / (ar*ar+aj*aj+1));
}

void CFm::multiply(int ar, int aj, int br, int bj, int *cr, int *cj)
{
    *cr = ar*br - aj*bj;
    *cj = aj*br + ar*bj;
}

int CFm::polar_discriminant(int ar, int aj, int br, int bj)
{
    int cr, cj;
    double angle;
    multiply(ar, aj, br, -bj, &cr, &cj);
    angle = atan2((double)cj, (double)cr);
    return (int)(angle / 3.14159 * (1<<14));
}

int CFm::fast_atan2(int y, int x)
/* pre scaled for int16 */
{
    int yabs, angle;
    int pi4=(1<<12), pi34=3*(1<<12);  // note pi = 1<<14
    if (x==0 && y==0) {
        return 0;
    }
    yabs = y;
    if (yabs < 0) {
        yabs = -yabs;
    }
    if (x >= 0) {
        angle = pi4  - pi4 * (x-yabs) / (x+yabs);
    } else {
        angle = pi34 - pi4 * (x+yabs) / (yabs-x);
    }
    if (y < 0) {
        return -angle;
    }
    return angle;
}

int CFm::polar_disc_fast(int ar, int aj, int br, int bj)
{
    int cr, cj;
    multiply(ar, aj, br, -bj, &cr, &cj);
    return fast_atan2(cj, cr);
}

void CFm::slotSetFilter(uint frequency) {
    CDemodulatorBase::slotSetFilter(frequency);
    if (filter != NULL) {
        filter->setSampleRate(22050);
        //qDebug() << "Initialize FIR filter frequency=" << frequency <<" mode=" << mode << " intfreq=" << intfreq << "\r\n";
        if (mode == eWFM) {
            deemph_a = (int)round(1.0/((1.0-exp(-1.0/(intfreq * 75e-6)))));
            filter->bandpass(5530.0,11000.0);
        }
        else {
            filter->lowpass(frequency);
        }
        //
        filter->convert();
    }
}

void CFm::deemph_filter(int16_t *buffer,int len)
{
    int i, d;
    // de-emph IIR
    // avg = avg * (1 - alpha) + sample * alpha;
    for (i = 0; i < len; i++) {
        d = buffer[i] - avg;
        if (d > 0) {
            avg += (d + deemph_a/2) / deemph_a;
        } else {
            avg += (d - deemph_a/2) / deemph_a;
        }
        buffer[i] = (int16_t)avg;
    }
}

void CFm::initPLL() {
    /*
    _phase = 0.0f;
    var norm = (float) (2.0 * Math.PI / _sampleRate);
    _frequencyRadian = _defaultFrequency * norm;
    _minFrequencyRadian = (_defaultFrequency - _range) * norm;
    _maxFrequencyRadian = (_defaultFrequency + _range) * norm;
    _alpha = 2.0f * _zeta * _bandwidth * norm;
    _beta = (_alpha * _alpha) / (4.0f * _zeta * _zeta);
    _phaseAdj = _phaseAdjM * _sampleRate + _phaseAdjB;
    _lockAlpha = (float) (1.0 - Math.Exp(-1.0 / (_sampleRate * _lockTime)));
    _lockOneMinusAlpha = 1.0f - _lockAlpha;
    */
}

void CFm::processPll(int i, int q) {
    /*
    var osc = Trig.SinCos(_phase);

    osc *= sample;
    var phaseError = -osc.ArgumentFast();

    _frequencyRadian += _beta * phaseError;

    if (_frequencyRadian > _maxFrequencyRadian)
    {
        _frequencyRadian = _maxFrequencyRadian;
    }
    else if (_frequencyRadian < _minFrequencyRadian)
    {
        _frequencyRadian = _minFrequencyRadian;
    }

    _phaseErrorAvg = _lockOneMinusAlpha * _phaseErrorAvg + _lockAlpha * phaseError * phaseError;
    _phase += _frequencyRadian + _alpha * phaseError;
    _phase %= (float) (2.0 * Math.PI);
    _adjustedPhase = _phase + _phaseAdj;

    return osc;
    */
}

QString CFm::getName() {
    return QString("CFm");
}
