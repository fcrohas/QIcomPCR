#include "ISound.h"

ISound::ISound(QObject *parent) :
    QThread(parent)
  ,pFile(NULL)
{
    soundStream = new CSoundStream();
}

ISound::~ISound()
{
    delete soundStream;
}

void ISound::SetDemodulator(CDemodulator *value)
{
    demod = value;
}

CDemodulator *ISound::GetDemodulator()
{
    return demod;
}

void ISound::DecodeBuffer(int16_t *buffer, int size)
{
    if (pFile != NULL)
    {
        sf_count_t bytewrite;
        bytewrite = sf_writef_short(pFile, buffer, size/2);
    }
    demod->slotDataBuffer(buffer, size);
    soundStream->encode(buffer,size);
}

void ISound::setRunning(bool value)
{

}

QHash<QString, int> ISound::getDeviceList()
{
    return QHash<QString, int>();
}

void ISound::selectInputDevice(QString device)
{

}

void ISound::selectOutputDevice(QString device)
{

}

bool ISound::Load(QString &file)
{
    return false;
}

void ISound::Record(QString &filename, bool start)
{
    // Sound file info struct
    if (start == true) {
        SF_INFO sfinfo;
        //memset(&sfinfo, 0, sizeof(SF_INFO));
        // Write standard wave file
        sfinfo.channels = 2;
        sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16 | SF_ENDIAN_LITTLE;
        sfinfo.samplerate = SAMPLERATE;
        //sfinfo.frames = 512;
        //sfinfo.sections = 1;
        //sfinfo.seekable =1;
        //Load file
        pFile = sf_open(filename.toLocal8Bit().data(), SFM_WRITE, &sfinfo);
        //sf_command (pFile, SFC_SET_UPDATE_HEADER_AUTO, NULL, SF_FALSE) ;
        //sf_command (pFile, SFC_SET_CLIPPING, NULL, SF_FALSE) ;

    } else {
        sf_command (pFile, SFC_UPDATE_HEADER_NOW, NULL, 0) ;
        sf_write_sync(pFile);
        sf_close(pFile);
        pFile = NULL;
    }
}
