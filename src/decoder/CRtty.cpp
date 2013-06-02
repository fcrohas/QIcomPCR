#include "CRtty.h"

CRtty::CRtty(QObject *parent, uint channel) :
    IDemodulator(parent),
    channel(0),
    frequency(4500),
    correlationLength(40),
    bandwidth(200),
    freqlow(1600.0), // 1600
    freqhigh(4000.0), // 4000
    baudrate(50),
    inverse(-1.0),
    letter(""),
    bitcount(0),
    accmark(0),
    accspace(0),
    started(false),
    sync(false),
    isletters(true),
    counter(0),
    mark_q(NULL),
    mark_i(NULL),
    space_i(NULL),
    space_q(NULL)
{
    // Table init
    xval = new double[getBufferSize()];
    yval = new double[getBufferSize()];
    corrmark = new double[getBufferSize()];
    corrspace = new double[getBufferSize()];
    avgcorr = new double[getBufferSize()];
    audioData[0] = new double[getBufferSize()];
    audioData[1] = new double[getBufferSize()];
    audioBuffer[0] = new double[getBufferSize()];
    audioBuffer[1] = new double[getBufferSize()];

    memset(audioBuffer[0],0,getBufferSize()*sizeof(double));
    memset(audioBuffer[1],0,getBufferSize()*sizeof(double));

    // Save local selected channel
    this->channel = channel;

    // Build filter
    winfunc = new CWindowFunc(this);
    winfunc->init(65);
    winfunc->rectangle();
    //int order = winfunc->kaiser(40,frequency,bandwidth,SAMPLERATE);
    // band pass filter
    fmark = new CFIR(this);
    fspace = new CFIR(this);
    flow = new CFIR(this);
    fmark->setWindow(winfunc->getWindow());
    fspace->setWindow(winfunc->getWindow());
    // arbitrary order for 200 Hz bandwidth
    // for mark frequency
    fmark->setOrder(64);
    fmark->setSampleRate(SAMPLERATE);
    fmark->bandpass(freqlow,bandwidth);

    // for space frequency
    fspace->setOrder(64);
    fspace->setSampleRate(SAMPLERATE);
    fspace->bandpass(freqhigh,bandwidth);

    // Low pass filter at the end
    flow->setOrder(64);
    flow->setSampleRate(SAMPLERATE);
    flow->lowpass(500.0);
    bit = 10.5+(SAMPLERATE/baudrate)/3.0; // We want it in sample unit
    qDebug() << "bit size for " << baudrate << " baud is " << bit;
    GenerateCorrelation(correlationLength);
}

CRtty::~CRtty()
{
    delete [] xval;
    delete [] yval;
    delete [] corrmark;
    delete [] corrspace;
    delete [] avgcorr;
    delete [] audioBuffer[1];
    delete [] audioBuffer[0];
    delete [] audioData[1];
    delete [] audioData[0];
    delete [] mark_i;
    delete [] mark_q;
    delete [] space_i;
    delete [] space_q;
    delete fmark;
    delete fspace;
    delete flow;
}

void CRtty::decode(int16_t *buffer, int size, int offset)
{
    // Convert to double and normalize between -1.0 and 1.0
    double peak = 0.0;
    for (int i=0 ; i < size; i++) {
        audioData[0][i] = buffer[i]*1.0/32768.0;
        audioData[1][i] = buffer[i]*1.0/32768.0;
    }
    // Pass band filter at frequency
    // With width of 200 Hz
    fmark->apply(audioData[0],getBufferSize());
    fspace->apply(audioData[1],getBufferSize());

    // Correlation of with selected frequency
    for(int i=size-1; i > 0; i--) { //
        // Init correlation value
        corrmark[i] = 0.0;
        corrspace[i] = 0.0;
        for (int j=correlationLength-1; j > 0 ; j--) {
            if (((i-j)>=0)) {
                corrmark[i]  += sqrt(pow(audioData[0][i-j] * mark_i[j],2)  + pow(audioData[0][i-j] * mark_q[j],2));
                corrspace[i] += sqrt(pow(audioData[1][i-j] * space_i[j],2) + pow(audioData[1][i-j] * space_q[j],2));
            } else {
                corrmark[i]  += sqrt(pow(audioBuffer[0][size-i-j] * mark_i[j],2)  + pow(audioBuffer[0][size-i-j] * mark_q[j],2));
                corrspace[i] += sqrt(pow(audioBuffer[1][size-i-j] * space_i[j],2) + pow(audioBuffer[1][size-i-j] * space_q[j],2));
            }
        }
        // Output results
        avgcorr[i] = (corrmark[i] - corrspace[i]) * inverse;
        // Moving Average filter result of correlation
#if 0
        if (i>4) {
            // Moving average filter
            yval[i] = avgcorr[i];
            for (int v=0; v < 4; v++) {
            yval[i] += yval[i-v]; // this is max value after correlation
            }
            yval[i] = yval[i] / 4.0;
        } else
#endif
            // Save to result buffer
            yval[i] = avgcorr[i];

        // Save to result buffer
        xval[i] = i;
    }

    // Copy back buffer
    memcpy(audioBuffer[0],audioData[0], size*sizeof(double));
    memcpy(audioBuffer[1],audioData[1], size*sizeof(double));

    // Apply lowpass filter
    flow->apply(yval,getBufferSize());

    // Decode baudo code
    // One bit timing is 1/baudrate
    for (int i=0; i< (size); i++) {
        // Counter started ?
        if (started) {
            // count how samples from start bit
            if (counter == bit / 2) { // half bit reached
                // start bit measurement
                sync = true;
            }
            counter +=1;
        }
        // reset all if bit count is wrong
        if (bitcount > DATABITS+1) {
            // reset
            started = false;
            sync = false;
            bitcount = 0;
            letter ="";
            counter = 0;
        }
        if (yval[i]<0.0) {
            // Start measure low state to detect start bit
            accspace += 1;
            if (accmark>0) {
                qDebug() << "mark timing " << accmark;
                accmark = 0;
            }
            //
            if ((started) && ((counter % bit) == 0) && (sync)) // if data started
            {
                bitcount +=1;
                letter.append("0");
            }
            //
            int length = abs(accspace -(bit*STARTBITS));
            if ((length <=10) && (!started) && (!sync)) // allow a margin of 50 samples
            {
                qDebug() << "Start bits detected";
                // Init bit counter
                bitcount = 1;
                counter = 0;
                letter = ""; //reset letter
                started = true;
                sync = false;
                // sync back to half space length
                counter = bit / 2;
            }
            //
        }
        if (yval[i]>0.0) {
            // this is a high state now
            accmark +=1;
            if (accspace>0) {
                qDebug() << "space timing " << accspace;
                accspace = 0;
            }
            //
            if ((started) && ((counter % bit) == 0) && (sync)) // if data started
            {
                // at each bit interval do the measure
                bitcount +=1;
                letter.append("1");
            }
            //
            int length = abs(accmark -(bit*STOPBITS));
            if (/*((length >=0) && (length <=2) &&*/ (started) && (sync) && (bitcount == DATABITS+1))// allow a margin of 50 samples
            {
                bitcount = 0;
                qDebug() << "Stop bits detected";
                if (letter.length() == DATABITS) {
                    bool ok;
                    if (letter.toInt(&ok,2) == 31) isletters = true;
                    else if (letter.toInt(&ok,2) == 27) isletters = false;
                    else {
                        if (isletters)
                            emit sendData(QString("%1").arg(QChar(lettersr[letter.toInt(&ok,2)])));
                        else
                            emit sendData(QString("%1").arg(QChar(figuresr[letter.toInt(&ok,2)])));
                    }
                }
                //emit sendData(QString("stop bit\r\n"));
                letter ="";
                started = false;
                sync = false;
                counter = 0;
            } // not a stop bit so
            //
        }
    }
    emit dumpData(xval, yval, size);
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
    freqlow = frequency - bandwidth/2.0;
    freqhigh = frequency + bandwidth/2.0;
    GenerateCorrelation(correlationLength);
    fspace->bandpass(freqhigh,bandwidth);
    fmark->bandpass(freqlow,bandwidth);
}


void CRtty::slotFrequency(double value)
{
    frequency = value * SAMPLERATE / 512.0; // SAMPLERATE / 512 and displaying graph is 0 to 128
    freqlow = frequency - bandwidth/2.0;
    freqhigh = frequency + bandwidth/2.0;
    GenerateCorrelation(correlationLength);
    fspace->bandpass(freqhigh,bandwidth);
    fmark->bandpass(freqlow,bandwidth);
}

void CRtty::setCorrelationLength(int value)
{
    correlationLength = value;
    GenerateCorrelation(correlationLength);
}

void CRtty::GenerateCorrelation(int length)
{
    if (mark_q == NULL) {
        mark_q = new double[length];
    }
    if (mark_i == NULL) {
        mark_i = new double[length];
    }
    if (space_q == NULL) {
        space_q = new double[length];
    }
    if (space_i == NULL) {
        space_i = new double[length];
    }
    double freq1 = 0.0;
    double freq2 = 0.0;
    correlationLength = length;
    for (int i=0; i< correlationLength;i++) {
        mark_i[i] = cos(freq1);
        mark_q[i] = sin(freq1);
        freq1 += 2.0*M_PI*freqlow/SAMPLERATE;
        space_i[i] = cos(freq2);
        space_q[i] = sin(freq2);
        freq2 += 2.0*M_PI*freqhigh/SAMPLERATE;
    }

}
