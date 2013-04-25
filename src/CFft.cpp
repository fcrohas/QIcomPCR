#include "CFft.h"

CFFT::CFFT(QObject *parent, int size) :
    QObject(parent)
  ,window(NULL)
{
    xval = new double[size];
    yval = new double[size];
    initBuffer(size);
    initFFT(size);
}

CFFT::~CFFT()
{
    if (window)
        delete [] window;
    delete [] xval;
    delete [] yval;
#ifdef FFTW
    fftw_destroy_plan(ch1);
    fftw_destroy_plan(ch2);
    fftw_free(out1);
    fftw_free(out2);
    fftw_free(in1);
    fftw_free(in2);
#else
    delete [] in1;
    delete [] in2;
#endif
}

void CFFT::initBuffer(int size)
{
#ifdef FFTW
    in1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    in2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
#else
    in1 = new SPUC::complex<double>[size];
    in2 = new SPUC::complex<double>[size];
#endif

}

void CFFT::initFFT(int size)
{
    // Describe plan
    ch1 = fftw_plan_dft_1d( size, in1, out1, FFTW_FORWARD, FFTW_ESTIMATE );
    ch2 = fftw_plan_dft_1d( size, in2, out2, FFTW_FORWARD, FFTW_ESTIMATE );
    blackman(size);
}

void CFFT::decode(int16_t *buffer, int size, double *xval, double *yval)
{
    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
#ifdef FFTW
        in1[j][0] = window[i] * buffer[i]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 1
        in2[j][0] = window[i] * buffer[i+1]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 2
        in1[j][1] = 0.0;
        in2[j][1] = 0.0;
#else
        in1[j].re = buffer[i]*1.0/32768.0; // set values in double between -1.0 and 1.0
        in1[j].im = 0.0;
        in2[j].re = buffer[i+1]*1.0/32768.0; // set values in double between -1.0 and 1.0
        in2[j].im = 0.0;
#endif
    }

#ifdef FFTW
    fftw_execute(ch1);
    fftw_execute(ch2);

#else
    SPUC::dft(in1,size);
    SPUC::dft(in2,size);
#endif

    // fill back spectrum buffer
    for (int i=0; i < size/2; i++) {
#ifdef FFTW
        yval[i] = sqrt(pow(out1[i][0],2) + pow(out1[i][1],2)); // Channel antenna 1
        yval[i+size/2] = sqrt(pow(out2[i][0],2) + pow(out2[i][1],2)); // Channel antenna 2
#else
        yval[i] = SPUC::magsq(in1[i]);
        yval[i+size/4] = SPUC::magsq(in2[i]);
#endif
        xval[i] = i;
        xval[i+size/2] = i+size/2;
    }
}

void CFFT::slotDataBuffer(int16_t *buffer, int size)
{
    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
#ifdef FFTW
        in1[j][0] = window[i] * buffer[i]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 1
        in2[j][0] = window[i] * buffer[i+1]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 2
        in1[j][1] = 0.0;
        in2[j][1] = 0.0;
#else
        in1[j].re = buffer[i]*1.0/32768.0; // set values in double between -1.0 and 1.0
        in1[j].im = 0.0;
        in2[j].re = buffer[i+1]*1.0/32768.0; // set values in double between -1.0 and 1.0
        in2[j].im = 0.0;
#endif
    }

#ifdef FFTW
    fftw_execute(ch1);
    fftw_execute(ch2);

#else
    SPUC::dft(in1,size);
    SPUC::dft(in2,size);
#endif

    // fill back spectrum buffer
    for (int i=0; i < size/2; i++) {
#ifdef FFTW
        yval[i] = sqrt(pow(out1[i][0],2) + pow(out1[i][1],2)); // Channel antenna 1
        yval[i+size/2] = sqrt(pow(out2[i][0],2) + pow(out2[i][1],2)); // Channel antenna 2
#else
        yval[i] = SPUC::magsq(in1[i]);
        yval[i+size/4] = SPUC::magsq(in2[i]);
#endif
        xval[i] = i;
        xval[i+size/2] = i+size/2;
    }

    emit sigRawSamples(xval, yval, size);
}

void CFFT::hamming(int size)
{
    if (window == NULL)
        window = new double[size];
    for (int i = 0; i < size; i++) {
        window[i] = 0.54 - 0.46 * cos(2*PI*i/(size-1));
    }
}


void CFFT::hann(int size)
{
    if (window == NULL)
        window = new double[size];
    for (int i = 0; i < size; i++) {
        window[i] = 0.5 * (1 - cos(2*PI*i/(size-1)));
    }
}

void CFFT::blackman(int size)
{
    if (window == NULL)
        window = new double[size];
    for (int i = 0; i < size; i++) {
        window[i] = 0.426591 - 0.496561*cos(2*PI*i/(size-1)) +0.076848*cos(4*PI*i/(size-1));
    }
}

void CFFT::rect(int size)
{
    if (window == NULL)
        window = new double[size];
    for (int i = 0; i < size; i++) {
        window[i] = 1.0;
    }
}
