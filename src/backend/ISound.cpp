/**********************************************************************************************
  Copyright (C) 2012 Fabrice Crohas <fcrohas@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

**********************************************************************************************/
#include "ISound.h"

ISound::ISound(QObject *parent) :
    QThread(parent)
  ,pFile(NULL)
{
    decoder = new QThread();
}

ISound::~ISound()
{
    decoder->terminate();
}

void ISound::SetDecoder(CDecoder *value, Mode mode)
{
    decod = value;
    decoder->start();
}

CDecoder *ISound::GetDecoder()
{
    return decod;
}

void ISound::DecodeBuffer(int16_t *buffer, int size)
{
    if (pFile != NULL)
    {
        sf_count_t bytewrite;
        bytewrite = sf_writef_short(pFile, buffer, size/2);
    }
    decod->setData(buffer,size);
    QMetaObject::invokeMethod(decod, "doWork");
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

void ISound::setChannel(uint value)
{

}

void ISound::Play(int16_t *buffer, int size) {

}

void ISound::Initialize() {

}
