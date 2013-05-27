#include "CRtty.h"

CRtty::CRtty(QObject *parent, uint channel) :
    IDemodulator(parent),
    channel(0),
    frequency(4500),
    correlationLength(40),
    bandwidth(250),
    freqlow(4263),
    freqhigh(4651),
    baudrate(50),
    inverse(1.0),
    letter(""),
    bitcount(0),
    started(false)
{
    // Table init
    xval = new double[getBufferSize()];
    corrmark = new double[getBufferSize()];
    corrspace = new double[getBufferSize()];
    avgcorr = new double[getBufferSize()];
    audioData[0] = new double[getBufferSize()];

    // Save local selected channel
    this->channel = channel;

    // Build filter
    winfunc = new CWindowFunc(this);
    winfunc->init(65);
    winfunc->hann();
    //int order = winfunc->kaiser(40,frequency,bandwidth,SAMPLERATE);
    // band pass filter
    fmark = new CFIR(this);
    fspace = new CFIR(this);
    fmark->setWindow(winfunc->getWindow());
    fspace->setWindow(winfunc->getWindow());
    // arbitrary order for 200 Hz bandwidth
    fmark->setOrder(64);
    fmark->setSampleRate(SAMPLERATE);
    fmark->bandpass(freqhigh,bandwidth);

    fspace->setOrder(64);
    fspace->setSampleRate(SAMPLERATE);
    fspace->bandpass(freqlow,bandwidth);

    bit = ((1/baudrate) * SAMPLERATE); // We want it in sample unit
    qDebug() << "bit size for " << baudrate << " baud is " << bit;
}

CRtty::~CRtty()
{
    delete [] xval;
    delete [] corrmark;
    delete [] corrspace;
    delete [] avgcorr;
    delete [] audioData[0];
    delete fmark;
    delete fspace;
}

void CRtty::decode(int16_t *buffer, int size, int offset)
{
    // Convert to double and normalize between -1.0 and 1.0
    double peak = 0.0;
    for (int i=0 ; i < size; i++) {
        audioData[0][i] = buffer[i]*1.0/32768.0;
    }
    // Pass band filter at frequency
    // With width of 200 Hz
    fmark->apply(audioData[0],getBufferSize());

    // Correlation of with selected frequency
    for(int i=0; i < size-correlationLength; i++) { //
        // Init correlation value
        corrmark[i] = 0.0;
        corrspace[i] = 0.0;
        corrmark[i]  = sqrt(pow(goertzel(&audioData[0][i],correlationLength, freqlow , SAMPLERATE),2));
        corrspace[i] = sqrt(pow(goertzel(&audioData[0][i],correlationLength, freqhigh, SAMPLERATE),2));
        if (i>0) corrmark[i] = (corrmark[i-1]+corrmark[i])/2.0; // this is max value after correlation
        if (i>0) corrspace[i] = (corrspace[i-1]+corrspace[i])/2.0; // this is max value after correlation
        avgcorr[i] = (corrmark[i] - corrspace[i]) * inverse;
        // Save to result buffer
        xval[i] = i;
    }

    // Fill end of buffer with 0 as the end is not used
    // Maybe suppress this if init is well done
    for (int i=0; i < correlationLength; i++) {
        avgcorr[(size-(int)correlationLength)+i] = 0.0;
        xval[(size-(int)correlationLength)+i] = (size-(int)correlationLength)+i;
    }
    // Low pass Filter to keep only baudot signal
    audioData[0] = avgcorr;

    // Decode baudo code
    // One bit timing is 1/baudrate
    accmark  = 0;
    accspace = 0;

    for (int i=0; i< size; i++) {
        if (audioData[0][i]<0.0) {
            // Start measure low state to detect start bit
            if (accmark>0) { // is this a stop bit ?
                if ((abs(accmark - (bit * STOPBITS)) <50.0)) // allow a margin of 50 samples
                {
                    qDebug() << "stop bit detected with letter " << letter;
                    // emit letter
                    letter.append("\r\n");
                    letter.append("Stop bits\r\n");
                    emit sendData(letter);
                    letter = ""; //reset letter
                    bitcount = 0;
                    started = false;
                } // not a stop bit so
                else {
                    // how much bit at 1 ?
                    int count = ceil((double)(accmark / bit));
                    bitcount += count; // Cut into number of bit
                    qDebug() << "bits  mark count "<< bitcount << "ceil("<< accmark <<" / bit)=" << count;
                    letter.append(QString(" %1M").arg(count));
                }
                accmark = 0;
            }
            else {
                accspace += 1;
            }
        }
        if (audioData[0][i]>0.0) {
            // this is a high state now
            if (accspace >0) { // do we come from low state ?
                // is this a start bit ?
                if (started)
                {
                    // how much bit at 0 ?
                    int count = ceil((double)(accspace / bit));
                    bitcount += count; // Cut into number of bit
                    qDebug() << "bits  space count "<< bitcount << "ceil("<< accspace <<" / bit)=" << count;
                    letter.append(QString(" %1S").arg(count));
                }

                if ((abs(accspace - (bit*STARTBITS)) <50.0) && (!started)) // allow a margin of 50 samples
                {
                    qDebug() << "start bit detected";
                    emit sendData(QString("Start bit\r\n"));
                    // Init bit counter
                    bitcount = 0;
                    letter = ""; //reset letter
                    started = true;
                }
                accspace = 0;
            }
            else {
                accmark +=1;
            }
        }
    }
    emit dumpData(xval, audioData[0], size);
}

uint CRtty::getDataSize()
{
    return 16;
}

uint CRtty::getBufferSize()
{
    return 4096;
}


uint CRtty::getChannel()
{
    return channel;
}

void CRtty::setThreshold(int value)
{

}

void CRtty::slotBandwidth(double value)
{
    bandwidth = value * SAMPLERATE / 512.0;
    // In rtty it is 400 Hz wide so
    freqlow = frequency - bandwidth/2;
    freqhigh = frequency + bandwidth/2;
}


void CRtty::slotFrequency(double value)
{
    frequency = value * SAMPLERATE / 512; // SAMPLERATE / 512 and displaying graph is 0 to 128
}

void CRtty::setCorrelationLength(int value)
{
    correlationLength = value;
}

double CRtty::goertzel(double *x, int N, double freq, int samplerate)
{
    double Skn, Skn1, Skn2;
    Skn = Skn1 = Skn2 = 0;

    for (int i=0; i<N; i++) {
        Skn2 = Skn1;
        Skn1 = Skn;
        Skn = 2*cos(2*M_PI*freq/samplerate)*Skn1 - Skn2 + x[i];
    }

    double WNk = exp(-2*M_PI*freq/samplerate); // this one ignores complex stuff
    //float WNk = exp(-2*j*PI*k/N);
    return (Skn - WNk*Skn1);
}
