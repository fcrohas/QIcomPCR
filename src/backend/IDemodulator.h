#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QObject>
#include <stdint.h>

class IDemodulator : public QObject
{
    Q_OBJECT
public:
    explicit IDemodulator(QObject *parent = 0);
    virtual void decode(uchar *buffer, int size, int offset);
    virtual void decode(int16_t *buffer, int size, int offset);
    virtual uint getDataSize();
    virtual uint getChannel();
    virtual uint getBufferSize();
    virtual void setThreshold(int value);
    virtual void setCorrelationLength(int value);


signals:
    void sendData(QString data);
    
public slots:
    void slotFrequency(double value);
    void slotBandwidth(int value);
};

#endif // IDEMODULATOR_H
