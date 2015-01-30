#include "CFm.h"

CFm::CFm(QObject *parent, Mode mode) :
    CDemodulatorBase(parent),
    avg(0),
    filter(NULL),
    filterIQ(NULL),
    deemph_a(1)
{
    //qDebug() << "CFM constructor...\r\n";
    // Build Window blackman
    winfunc = new CWindowFunc(this);
    winfunc->init(32);
    winfunc->blackmanharris();
    // Low pass IQ FIR filter
    filterIQ = new CFIR<int16_t>();
    filterIQ->setOrder(32);
    filterIQ->setWindow(winfunc->getWindow());
    // Set FIR filter
    // Build Bandpass filter
    filter = new CFIR<int16_t>();
    filter->setOrder(32);
    filter->setWindow(winfunc->getWindow());
    this->mode = mode;
    slotSetFilter(filterfreq);
    //qDebug() << "CFM constructor Mode="<< mode << "\r\n";
    _mask = ~(-1 << 16);
    int sampleCount = _mask + 1;
    sinPtr = new float[sampleCount];
    cosPtr = new float[sampleCount];
    _indexScale = sampleCount / (2.0f*M_PI);
    for (int i = 0; i < sampleCount; i++)
    {
        sinPtr[i] = (float) sin((i + 0.5f) / sampleCount * (2*M_PI));
        cosPtr[i] = (float) cos((i + 0.5f) / sampleCount * (2*M_PI));
    }

    for (float angle = 0.0f; angle < M_PI*2.0; angle += M_PI/2.0)
    {
        sinPtr[(int) (angle * _indexScale) & _mask] = (float) sin(angle);
        cosPtr[(int) (angle * _indexScale) & _mask] = (float) cos(angle);
    }
}

void CFm::doWork() {
    update.lock();
    // Signal to noise ratio
    int prev = 0;
    int difSqrSum = 0;
    // Decimate to get 250K sample rate
    len = CDemodulatorBase::downsample(buffer,len,decimation);
    // Low pass filtering IQ samples
    filterIQ->applyIQ(buffer,len);
    // Demodulate with discriminator
    int16_t pr = pre_real;
    int16_t pj = pre_img;
    int pcm = 0;
    // demodulate FM
    for (int i = 0; i < len; i += 2) {
        if (mode == eWFM) {
            // Demodulate
            pcm = polar_disc_fast(buffer[i], buffer[i+1], pr, pj);
            // Detect 19Khz
            //pcm = processPll(buffer[i], buffer[i+1]);
        }
        else
            pcm = polar_discriminant(buffer[i], buffer[i+1], pr, pj);
        pr = buffer[i];
        pj = buffer[i+1];
        difSqrSum += (prev - pcm) * (prev - pcm);
        buffer[i/2] = (int16_t)pcm;
        prev = pcm;
    }
    // Update new length for one channel
    len = len/2;
    // Update snr
    snr = abs(1 - sqrt(difSqrSum / len));
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
    qDebug() << " phase error=" << phaseErrorAvg;
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
    // Initiliaze PLL
    initPLL(12500,25000);
    // audio filtering
    if (filter != NULL) {
        filter->setSampleRate(22050);
        //qDebug() << "Initialize FIR filter frequency=" << frequency <<" mode=" << mode << " intfreq=" << intfreq << "\r\n";
        if (mode == eWFM) {
            deemph_a = (int)round(1.0/((1.0-exp(-1.0/(intfreq * 75e-6)))));
            filter->bandpass(5530.0,11000.0);
        }
        else {
            filter->lowpass(22050/2);
        }
    }
    // IQ filtering
    if (filterIQ != NULL) {
        // Sample rate after decimation
        filterIQ->setSampleRate(intfreq);
        filterIQ->lowpass(frequency);
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

void CFm::initPLL(float frequency, float bandwidth) {
    phase = 0.0f;
    zeta = 0.707f;
    phaseAdjustM = 0.0f;
    phaseAdjustB = -1.75f;
    //phaseErrorAvg = 1.0f;
    float norm = 2.0f * M_PI / intfreq;
    centerf = frequency * norm;
    lowerf = (frequency - (bandwidth/2.0f)) * norm;
    higherf = (frequency + (bandwidth/2.0f)) * norm;
    alpha = 2.0f * zeta * bandwidth * norm;
    beta = (alpha * alpha) / (4.0f * zeta * zeta);
    phaseAdjust = phaseAdjustM * intfreq + phaseAdjustB;
    lockAlpha = (float) (1.0f - exp(-1.0 / (intfreq * lockTime)));
}

double CFm::processPll(int i, int q) {
    int index = (int) (phase * _indexScale) & _mask;
    float oscI = cosPtr[index];
    float oscQ = sinPtr[index];

    oscI = oscI * i - oscQ * q;
    oscQ = oscQ * i + oscI * q;
    float phaseError = -atan2(oscQ,oscI);

    centerf += beta * phaseError;

    if (centerf > higherf)
    {
        centerf = higherf;
    }
    else if (centerf < lowerf)
    {
        centerf = lowerf;
    }

    phaseErrorAvg = (1.0f - lockAlpha) * phaseErrorAvg + lockAlpha * phaseError * phaseError;
    phase += centerf + alpha * phaseError;
    phase = fmodf((float)(2.0f * M_PI),phase);
    adjustedPhase = phase + phaseAdjust;
    //if (phaseErrorAvg < 1.0f) qDebug() << "Pll locked " << phaseErrorAvg;
    return sqrtf(oscI*oscI+oscQ*oscQ);
}

QString CFm::getName() {
    return QString("CFm");
}
