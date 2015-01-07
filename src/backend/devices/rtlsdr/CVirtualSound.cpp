#include "CVirtualSound.h"

CVirtualSound::CVirtualSound(QObject *parent) :
    ISound(parent)
{
    running = true;
}

CVirtualSound::~CVirtualSound() {

}

void CVirtualSound::Record(QString &filename, bool start)
{
    ISound::Record(filename, start);
}

void CVirtualSound::setRunning(bool value)
{
    running = value;
}

void CVirtualSound::DecodeBuffer(int16_t *buffer, int size)
{
    ISound::DecodeBuffer(buffer,size);
}

bool CVirtualSound::Load(QString &fileName)
{
    return false;
}

void CVirtualSound::setChannel(uint value)
{
}

QHash<QString,int> CVirtualSound::getDeviceList() {
    deviceList.clear();
    return deviceList;
}

void CVirtualSound::run() {

}

void CVirtualSound::terminate() {

}

