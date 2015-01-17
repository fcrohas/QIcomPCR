#include "CFm.h"

CFm::CFm(QObject *parent, Mode mode) :
    IDemodulator(parent),
    avg(0),
    filter(NULL)
{
    qDebug() << "CFM constructor\r\n";
    // Build Window blackman
    winfunc = new CWindowFunc(this);
    winfunc->init(108);
    winfunc->blackmanharris();
    // Set FIR filter
    // Build Bandpass filter
    filter = new CFIR<double>();
    filter->setOrder(108);
    filter->setWindow(winfunc->getWindow());
    slotSetFilter(filterfreq);
}

void CFm::doWork() {
    update.lock();
    int intermediatefreq = samplerate/decimation;
    len = IDemodulator::downsample(buffer,len);
    int16_t pr = pre_real;
    int16_t pj = pre_img;
    int pcm = 0;
    // demodulate FM
    for (int i = 0; i < len; i += 2) {
        //pcm = esbensen(buffer[i], buffer[i+1], pr, pj);
        pcm = polar_disc_fast(buffer[i], buffer[i+1], pr, pj);
        pr = buffer[i];
        pj = buffer[i+1];
        buffer[i/2] = (int16_t)pcm;
    }
    // Update new length for one channel
    len = len/2;
    // Deemphasis filter if WBFM mode
    if (mode == eWFM)
        deemph_filter(buffer,len);
    // resample for sound output
    len = IDemodulator::resample(buffer,len,intermediatefreq);
    // Apply audio filter
    filter->apply(buffer,len);
    // send sound to queue
    IDemodulator::processSound(buffer,len);
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
    IDemodulator::slotSetFilter(frequency);
    deemph_a = (int)round(1.0/((1.0-exp(-1.0/(filterfreq * 75e-6)))));
    if (filter != NULL) {
        filter->setSampleRate(22050);
        //filter->bandpass(4030.0,8000.0);
        filter->lowpass(4000.0);
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
