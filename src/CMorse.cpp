#include "CMorse.h"
#include <QDebug>

CMorse::CMorse(QObject *parent, uint channel) :
    IDemodulator(parent)
  ,frequency(6000)
{
    // On creation allocate all buffer at maximum decoder size
    mark_i = new double[getBufferSize()];
    mark_q = new double[getBufferSize()];
    xval = new double[getBufferSize()];
    yval = new double[getBufferSize()];
    corr = new double[getBufferSize()];
    mark = new double[getBufferSize()];
    space = new double[getBufferSize()];
    audioData[0] = new double[getBufferSize()];

    // Calculate correlation length
    correlationLength = SAMPLERATE/frequency; // Default creation is 6Khz

    double freq = 0.0;
    // Generate Correlation for this frequency
    for (int i=0; i< correlationLength;i++) {
        mark_i[i] = cos(freq);
        mark_q[i] = sin(freq);
        freq += 2.0*M_PI*frequency/SAMPLERATE;
    }

    //Init hash table with morse code
    for (int i=0; i < sizeof(cw_table)/sizeof(CW_TABLE)-1; i++) {
        qDebug() << "init hash table with " << QString(cw_table[i].chr) << " symbol " << QString(cw_table[i].rpr);
        code.insert(QString(cw_table[i].rpr),cw_table[i].chr);
    }
    // Save local selected channel
    this->channel = channel;
    // Initialize Bandpass filter
    fmorse = new Dsp::SmoothedFilterDesign<Dsp::Butterworth::Design::BandPass <4>, 1, Dsp::DirectFormII> (1024);
    Dsp::Params paramsMorse;
    paramsMorse[0] = SAMPLERATE;
    paramsMorse[1] = 4; // order
    paramsMorse[2] = frequency; // center frequency
    paramsMorse[3] = 200; // band width
    fmorse->setParams(paramsMorse);

}

void CMorse::decode(int16_t *buffer, int size, int offset)
{
    // Convert to double and normalize between -1.0 and 1.0
    double peak = 0.0;
    for (int i=0 ; i < size; i++) {
        audioData[0][i] = buffer[i]*1.0/32768.0;
    }
    // Pass band filter at frequency
    // With width of 200 Hz
    fmorse->process(getBufferSize(), audioData);

    // Correlation of with selected frequency
    for(int i=0; i < size-correlationLength; i++) {
        // Init correlation value
        corr[i] = 0.0;
        // Correlate en shift over correlation length
        for (int j=0; j<correlationLength; j++) {
            corr[i] += sqrt( pow(audioData[0][i+j] * mark_i[j],2) + pow(audioData[0][i+j] * mark_q[j],2) );
        }
        // Detect maximum correlation peak
        if (corr[i]>peak) peak = corr[i]; // this is max value after correlation
        // Save to result buffer
        yval[i] = corr[i];
        xval[i] = i;

    }
    // Fill end of buffer with 0 as the end is not used
    // Maybe suppress this if init is well done
    for (int i=0; i < correlationLength; i++) {
        yval[(size-(int)correlationLength)+i] = 0.0;
        xval[(size-(int)correlationLength)+i] = (size-(int)correlationLength)+i;
    }
    // Now calculation of timing
    double agc = peak / 2; // average value per buffer size
    if (agc<0.25) agc=0.25; // minimum detection signal is 0.5
    int accup = 0; // accumulator sample high state
    int acclow = 0; // accumulator sample low state
    int marks = 0; // Count edge
    int spaces = 0; // count space
    // Detect High <-> low state and timing
    for (int i=0; i<size-(int)correlationLength; i++) {
        // if > then it is mark
        if (yval[i] >agc) {
            // Start to coutn time in sample
            if (acclow > 0) {
                double time = acclow * 1000.0 /22050.0;
                if (time > 10) {// only accept if impulse is > 5ms
                    //emit sendData(QString("low state for %1 ms").arg(time)); // print in millisecond result
                    acclow = 0;
                    space[spaces] = time;
                    // try to calculate new symbol
                    CheckLetterOrWord(spaces, marks);
                    //translate(edge); // Try to determine symbol of this position
                    spaces++;
                    accup++;
                } else { acclow=0; accup++; }
            } else
                accup++;
        }
        // if we were in high state then it is end, so dump result
        if (yval[i] < agc)  {
            // How much milliseconds ?
            if (accup >0 ) {
                double time = accup * 1000.0 /22050.0;
                if (time > 10) {// only accept if impulse is > 5ms
                    //emit sendData(QString("high state for %1 ms").arg(time)); // print in millisecond result
                    // reset acc
                    accup = 0;
                    mark[marks] = time;
                    // try to calculate new symbol only using high state
                    translate(marks,spaces); // Try to determine symbol of this position
                    marks++;
                    acclow++;
                } else  { accup=0; acclow++; }
            } else
                acclow++;
        }
    }
    // Send correlated signal to scope
    emit dumpData(xval,yval,getBufferSize());
}

uint CMorse::getDataSize()
{
    // wanted buffer size per sample
    // 16 bit for this one
    return 16;
}

uint CMorse::getChannel()
{
    // channel used by this demodulator
    return channel;
}

uint CMorse::getBufferSize()
{
    // Buffer size
    return 16384;
}

void CMorse::slotFrequency(double value)
{
    // Calculate frequency value from selected FFT bin
    // only half samplerate is available and FFT is set to 128 per channel
    frequency = value * SAMPLERATE / 2 / 128; // SAMPLERATE / 128 usable bin per channel
    // New correlation length as frequency selected has changed
    correlationLength = SAMPLERATE/1200;

    // Generate Correlation for this frequency
    double freq = 0.0;
    for (int i=0; i< correlationLength;i++) {
        mark_i[i] = cos(freq);
        mark_q[i] = sin(freq);
        freq += 2.0*M_PI*frequency/SAMPLERATE;
    }

    // New Bandpass filter params
    Dsp::Params paramsMorse;
    paramsMorse[0] = SAMPLERATE;
    paramsMorse[1] = 4; // order
    paramsMorse[2] = frequency; // center frequency
    paramsMorse[3] = 200; // band width
    fmorse->setParams(paramsMorse);

    qDebug() << "Correlation generated for frequency " << frequency << " hz";

}

float CMorse::goertzel(int16_t *x, int N, double freq, int samplerate)
{
    float Skn, Skn1, Skn2;
    Skn = Skn1 = Skn2 = 0;

    for (int i=0; i<N; i++) {
    Skn2 = Skn1;
    Skn1 = Skn;
    Skn = 2*cos(2*M_PI*freq/samplerate)*Skn1 - Skn2 + x[i];
    }

    float WNk = exp(-2*M_PI*freq/samplerate); // this one ignores complex stuff
    //float WNk = exp(-2*j*PI*k/N);
    return (Skn - WNk*Skn1);
}

void CMorse::translate(int position, int position2)
{
    // start only if we have two edge
    if (position > 0) {
        // Compute ratio between
        // high state timing
        double ratio = mark[position] / mark[position-1];
        if (ratio>1.2) // the two last symbol sequence are -.
        {
            // if already a symbol is here and is a jocker
            if ((symbols.length()>0) && (symbols.at(symbols.length()-1)) == QChar('*'))
            {
                // replace it
                symbols[symbols.length()-1] = QChar('-');
                symbols += QChar('.');
            }
            else
                symbols += QChar('.');
        }
        else if (ratio < 0.12) // the two last symbol sequence are .-
        {
            // if already a symbol is here and is a jocker
            if ((symbols.length()>0) && (symbols.at(symbols.length()-1)) == QChar('*'))
            {
                // replace it
                symbols[symbols.length()-1] = QChar('.');
                symbols += QChar('-');
            }
            else
                symbols += QChar('-');
        } else // no one match then symbol is same as previous
        {
            if (symbols.length()>0) // same as previous symbol
                symbols += QChar(symbols.at(symbols.length()-1));
            else // set a jocker
            {
                // use space / mark ratio to know if it is a dash or a point
                double symRatio = 0.0;
                if (position2>0) {
                    symRatio = space[position2]/mark[position];
                    if (symRatio < 0.0)
                        symbols += QChar('-');
                    else
                        symbols += QChar('.');
                }
                else
                    symbols += QChar('*');
            }
        }

    } else symbols += QChar('*');
}

void CMorse::CheckLetterOrWord(int position, int position2)
{
    if (position > 0) {
        // Compute ratio between
        // high state timing
        double ratio = space[position] / space[position-1];
        if (ratio > 2.0) // a space between letter is detected
        {
            if (code.contains(symbols))
                emit sendData(QString("Decoded char is %1 with symbols %2").arg(QChar(code.value(symbols))).arg(symbols));
            else
                emit sendData(QString("Unknown char"));
            symbols = QString("");
        }
    }


}
