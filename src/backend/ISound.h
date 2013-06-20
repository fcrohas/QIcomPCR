#ifndef ISOUND_H
#define ISOUND_H

#include <QObject>
#include <QThread>
#include <sndfile.h>
#include "CDemodulator.h"

class ISound : public QThread
{
    Q_OBJECT
public:
    explicit ISound(QObject *parent = 0);
    ~ISound();

    // Demodulator
    void SetDemodulator(CDemodulator *value);
    CDemodulator *GetDemodulator();

    virtual void DecodeBuffer(int16_t *buffer, int size);
    virtual void setRunning(bool value);
    virtual QHash<QString,int> getDeviceList();
    virtual void selectInputDevice(QString device);
    virtual void selectOutputDevice(QString device);
    virtual bool Load(QString &file);
    virtual void Record(QString &filename, bool start);

signals:
    
public slots:

private:
    CDemodulator *demod;
    SNDFILE*	pFile;
    QString data;
};

#endif // ISOUND_H
