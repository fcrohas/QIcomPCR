#include "CMorse.h"
#include <QDebug>

CMorse::CMorse(QObject *parent, uint channel) :
    IDemodulator(parent)
{
    // Init sinus and cos for correlation
    correlationLength = 22050 /1200; // For 1200 baud
    space_i = new double[correlationLength];
    space_q = new double[correlationLength];
    mark_i  = new double[correlationLength];
    mark_q  = new double[correlationLength];
    double f = 0.0;
    for (int i=0; i< correlationLength;i++) {
        space_i[i] = cos(f);
        space_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_SPACE/SAMPLERATE;
        //qDebug() << "space[" << i << "]=" << space_i[i];

    }
    f = 0.0;
    for (int i=0; i< correlationLength;i++) {
        mark_i[i] = cos(f);
        mark_q[i] = sin(f);
        f += 2.0*M_PI*FREQ_MARK/SAMPLERATE;
    }

    this->channel = channel;
}

void CMorse::decode(int16_t *buffer, int size, int offset)
{
    // Windowing
    //spuc::hamming(buffer, 512,0.5, 1.2);
    // loop on sampling
    for(int i=0; i < size; i+=2) {
        // Compute correlation for frequency
        for (int j=0; j<correlationLength; j++) {
            double value = buffer[i] * mark_i[j] + buffer[i] * mark_q[j];

        }
    }
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
    return 512;
}
