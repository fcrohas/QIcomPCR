#include "CIir.h"

CIIR::CIIR(QObject *parent) :
    QObject(parent),
    bufferin(NULL),
    bufferout(NULL),
    high(false),
    low(false),
    band(false),
    stop(false),
    update(false)
{
    a0l = 0.0;
    a1l = 0.0;
    a2l = 0.0;
    b1l = 0.0;
    b2l = 0.0;
    a0h = 0.0;
    a1h = 0.0;
    a2h = 0.0;
    b1h = 0.0;
    b2h = 0.0;
}

CIIR::~CIIR()
{
    // Reset buffer
    delete [] bufferin;
    delete [] bufferout;
    bufferin = NULL;
    bufferout = NULL;
}

void CIIR::setWindow(double *win)
{

}

// Bessel IIR filter
void CIIR::lowpass(double frequency)
{
    update = true;
    // 3 dB cutoff correction
    double c = pow(pow(pow(2.0,1/N) - 0.75,0.5) - 0.5,-0.5)/sqrt(3.0);
    // polynomial coefficients
    double g = 3.0;
    double p = 3.0;
    // corrected cutoff frequency
    double fc = c * frequency/fs;
    // warp cutoff freq from analog to digital domain
    double w0 = tan(M_PI*fc);

    // filter coefficient calculations
    double k1 = w0 * p;
    double k2 = pow(w0,2) * g;
    a0l = k2 / (1 + k1 + k2);
    a1l = 2.0*a0l;
    a2l = a0l;
    b1l = 2.0*a0l * (1 / k2 - 1);
    b2l = 1 - (a0l + a1l + a2l + b1l);
    // lowpass filter is called
    low = true;
    qDebug() << "coeff a0l=" << a0l << " a1l="<< a1l <<" a2l="<< a2l <<" b1l="<<b1l <<" b2l=" <<b2l <<" fc=" <<fc;
    // Reset buffer
    delete [] bufferin;
    delete [] bufferout;
    bufferin = NULL;
    bufferout = NULL;
    update = false;
}

void CIIR::highpass(double frequency)
{
    update = true;
    // 3 dB cutoff correction
    double c = pow(pow(pow(2.0,1/N) - 0.75,0.5) - 0.5,0.5)*sqrt(3.0);
    // polynomial coefficients
    double g = 3.0;
    double p = 3.0;
    // corrected cutoff frequency
    double fc = 0.5 - (c * frequency/fs); //
    // warp cutoff freq from analog to digital domain
    double w0 = tan(M_PI*fc);

    // filter coefficient calculations
    double k1 = w0 * p;
    double k2 = pow(w0,2.0) * g;
    a0h = k2 / (1 + k1 + k2);
    a1h = 2.0*a0h*-1.0;
    a2h = a0h;
    b1h = 2.0*a0h * (1 / k2 - 1.0) * -1.0;
    b2h = 1.0 - (a0h + a1h + a2h + b1h);
    // highpass filter is called
    high = true;
    // Reset buffer
    delete [] bufferin;
    delete [] bufferout;
    bufferin = NULL;
    bufferout = NULL;
    qDebug() << "coeff a0h=" << a0h << " a1h="<< a1h <<" a2h="<< a2h <<" b1h="<<b1h <<" b2h=" <<b2h <<" fc=" <<fc;
    update = false;
}

void CIIR::bandpass(double centerfreq, double bandwidth)
{
    double lowfreq = centerfreq - bandwidth / 2.0;
    double highfreq = centerfreq + bandwidth / 2.0;
    // calculate lowpass coeff
    lowpass(lowfreq);
    //calculate hoghpass coeff
    highpass(highfreq);
    high = false;
    low = false;
    band = true;
}


void CIIR::stopband(double centerfreq, double bandwidth)
{
    double lowfreq = centerfreq + bandwidth / 2.0;
    double highfreq = centerfreq - bandwidth / 2.0;
    // calculate lowpass coeff
    lowpass(lowfreq);
    //calculate hoghpass coeff
    highpass(highfreq);
    high = false;
    low = false;
    stop = true;
}

void CIIR::setOrder(int value)
{
    // Only even order filter
    N = value / 2;
}

void CIIR::setSampleRate(double value)
{
    fs = value;
}

void CIIR::apply(double *&in, int size)
{
    if (update)
        return;
     // yk = a0 xk + a1 xk−1 + a2 xk−2 + b1 yk−1 + b2 yk−2
    if (bufferin == NULL) {
        bufferin = new double[size];
        memset(bufferin,0,size*sizeof(double));
    }
    if (bufferout == NULL) {
        bufferout = new double[size];
        memset(bufferout,0,size*sizeof(double));
    }
    // Pass for order level
    int pass = 0;
    while( pass < N) {
        // loop on buffer and apply filter
        for (int i=0; i<size; i++) {
            double sample = 0.0;
            // act as a ring buffer for current and previous call
            bufferin[i] = in[i];
            // lowpass iir filter
            if (low == true) {
                if (i == 0)
                    sample = a0l*bufferin[0] + a1l*bufferin[size-1] + a2l*bufferin[size-2] + b1l*bufferout[size-1] + b2l*bufferout[size-2]; // use previous buffer call values
                else if (i==1)
                    sample = a0l*bufferin[1] + a1l*bufferin[0] + a2l*bufferin[size-1] + b1l*bufferout[0] + b2l*bufferout[size-1]; // use previous buffer call values
                else
                    sample = a0l*bufferin[i] + a1l*bufferin[i-1] + a2l*bufferin[i-2] + b1l*bufferout[i-1] + b2l*bufferout[i-2]; // use current buffer values
            }
            // highpass iir filter
            if (high == true) {
                if (i == 0)
                    sample = a0h*bufferin[0] + a1h*bufferin[size-1] + a2h*bufferin[size-2] + b1h*bufferout[size-1] + b2h*bufferout[size-2]; // use previous buffer call values
                else if (i==1)
                    sample = a0h*bufferin[1] + a1h*bufferin[0] + a2h*bufferin[size-1] + b1h*bufferout[0] + b2h*bufferout[size-1]; // use previous buffer call values
                else
                    sample = a0h*bufferin[i] + a1h*bufferin[i-1] + a2h*bufferin[i-2] + b1h*bufferout[i-1] + b2h*bufferout[i-2]; // use current buffer values
            }
            // Save
            //qDebug() << "sample="<< sample;
            bufferout[i] = sample;
            // pass level
            in[i] = bufferout[i];
        }
        pass++;
    }
}

