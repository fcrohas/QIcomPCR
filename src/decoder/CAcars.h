#ifndef CACARS_H
#define CACARS_H

#include <QObject>
#include <IDemodulator.h>
#define ACARSBUFFER 16384
extern "C" {
    #include <libacarsd.h>
}

class CAcars : public IDemodulator
{
    Q_OBJECT
public:
    explicit CAcars(QObject *parent = 0, uint channel=1, int bufferSize = ACARSBUFFER, int sampleRate=22050, int passes=8);
    void decode(uchar *buffer, int size, int offset);
    uint getDataSize();
    uint getChannel();
    uint getBufferSize();

public slots:
    void slotFrequency(double value);

private:
    libacarsd *L;
    uint channel;
    
};

#endif // CACARS_H
