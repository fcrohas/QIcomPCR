#ifndef CVIRTUALSOUND_H
#define CVIRTUALSOUND_H

#include "ISound.h"

class CVirtualSound : public ISound
{
    Q_OBJECT
public:
    explicit CVirtualSound(QObject *parent = 0);
    ~CVirtualSound();
    bool Load(QString &fileName);
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);
    QHash<QString, int> getDeviceList();
    void Record(QString &filename, bool start);
signals:

public slots:
    void setChannel(uint value);
private:
    void run();
    void terminate();
    int16_t buffer[BUFFER_SIZE]; // 16 bits pulse audio buffer
    bool running;
    QHash<QString,int> deviceList;
};

#endif // CVIRTUALSOUND_H
