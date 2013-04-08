#include "CMorse.h"
#include <QDebug>

CMorse::CMorse(QObject *parent, uint channel) :
    IDemodulator(parent)
  ,frequency(0)
{
    // Init sinus and cos for correlation
    correlationLength = 256; // For 1200 baud
    space_i = new double[correlationLength];
    space_q = new double[correlationLength];
    double f = 0.0;
    for (int i=0; i< correlationLength;i++) {
        space_i[i] = cos(f);
        space_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_SPACE/SAMPLERATE;
        //qDebug() << "space[" << i << "]=" << space_i[i];

    }
    xval = new double[256];
    yval = new double[256];
    window = new double[256];
    corr = new double[256];
    SPUC::hamming(window, 256,0.5, 1.2);
    this->channel = channel;
}

void CMorse::decode(int16_t *buffer, int size, int offset)
{

    // loop on sampling
    for(int i=0; i < size; i++) {
        // Haming window
        //buffer[i] = buffer[i] * window[i];

        // Compute correlation for frequency
        for (int j=0; j<correlationLength; j++) {
            corr[i] += sqrt(pow(buffer[i]*1.0/32768 * space_i[j],2) + pow(buffer[i]*1.0/32768 * space_q[j],2));
            //qDebug() << "correlation " << abs(corr[i]);
        }
        yval[i] = corr[i] * 1.0;
        xval[i] = i;
    }
    emit dumpData(xval,yval,256);
}

uint CMorse::getDataSize()
{
    return 16;
}

uint CMorse::getChannel()
{
    return channel;
}

uint CMorse::getBufferSize()
{
    return 256;
}

void CMorse::slotFrequency(int value)
{
    frequency = value * SAMPLERATE / (512/2); // SAMPLERATE / (N Sample / 2) , give frequency per fft bin
    double f = 0.0;
    // Generate Correlation for this frequency
    for (int i=0; i< 256;i++) {
        space_i[i] = cos(f);
        space_q[i] = sin(f);
        f += 2.0*M_PI*frequency/SAMPLERATE;
    }
    qDebug() << "Correlation generated for frequency " << frequency << " hz";

}
