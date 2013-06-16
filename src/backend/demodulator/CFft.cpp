#include "CFft.h"

CFFT::CFFT(QObject *parent, int size) :
    QObject(parent)
  ,window(NULL)
  ,winfunc(NULL)
  ,bufferBlock(0)
  ,channelSize(FRAME_SIZE)
  ,update(false)
{
    xval = new double[size];
    yval = new double[size];
    initBuffer(size);
    initFFT(size);
    fftsize = size;
    setWindow(CFFT::Blackman, size);
}

CFFT::~CFFT()
{
    delete winfunc;
    if (window)
        delete [] window;
    delete [] xval;
    delete [] yval;
    fftw_destroy_plan(ch1);
    fftw_destroy_plan(ch2);
    fftw_free(out1);
    fftw_free(out2);
    fftw_free(in1);
    fftw_free(in2);
}

void CFFT::initBuffer(int size)
{
    in1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    in2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out1 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
    out2 = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * size);
}

void CFFT::initFFT(int size)
{
    // Describe plan
    ch1 = fftw_plan_dft_1d( size, in1, out1, FFTW_FORWARD, FFTW_MEASURE );
    ch2 = fftw_plan_dft_1d( size, in2, out2, FFTW_FORWARD, FFTW_MEASURE );
}

void CFFT::decode(int16_t *buffer, int size, double *xval, double *yval)
{
    if (update == true)
        return;

    // fill complex
    for (int i=0, j=0; i < size; i+=2,j++) {
        in1[j+ bufferBlock * channelSize][0] = buffer[i]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 1
        in2[j+ bufferBlock * channelSize][0] = buffer[i+1]*1.0/32768.0; // set values in double between -1.0 and 1.0; channel 2
        in1[j][1] = 0.0;
        in2[j][1] = 0.0;
    }
    // if we reach the FFT Size
    if (bufferBlock * channelSize + channelSize == fftsize) {

        for (int i=0; i< fftsize; i++) {
            in1[i][0] = window[i] * in1[i][0];
            in2[i][0] = window[i] * in2[i][0];
        }

        fftw_execute(ch1);
        fftw_execute(ch2);

        // fill back spectrum buffer
        for (int i=0; i < fftsize/2; i++) {
            yval[i] = sqrt(pow(out1[i][0],2) + pow(out1[i][1],2)); // Channel antenna 1
            yval[i+fftsize/2] = sqrt(pow(out2[i][0],2) + pow(out2[i][1],2)); // Channel antenna 2
            xval[i] = i;
            xval[i+fftsize/2] = i+fftsize/2;
        }
        // Reinit buffer block
        bufferBlock = -1;

    }

    bufferBlock++;
}

void CFFT::setWindow(windowFunction fct, int size)
{
    update = true;
    if (winfunc == NULL)
        winfunc = new CWindowFunc(this);
    winfunc->init(size);
    switch(fct) {
        case BlackmanHarris : winfunc->blackmanharris(); break;
        case Blackman  : winfunc->blackman(); break;
        case Hann      : winfunc->hann(); break;
        case Hamming   : winfunc->hamming(); break;
        case Rectangle : winfunc->rectangle(); break;
        default        : winfunc->rectangle(); break;
    }
    window = winfunc->getWindow();
    update = false;
}
