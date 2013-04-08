#ifndef ISOUND_H
#define ISOUND_H

#include <QObject>
#include <QThread>
#include "CDemodulator.h"

class ISound : public QThread
{
    Q_OBJECT
public:
    explicit ISound(QObject *parent = 0);

    // Demodulator
    void SetDemodulator(CDemodulator *value);
    CDemodulator *GetDemodulator();

    virtual void DecodeBuffer(int16_t *buffer, int size);
    virtual void setRunning(bool value);
    virtual QHash<QString,int> getDeviceList();
    virtual void selectInputDevice(QString device);
    virtual void selectOutputDevice(QString device);
    virtual bool Load(QString &file);

signals:
    
public slots:

private:
    CDemodulator *demod;

};

#endif // ISOUND_H
