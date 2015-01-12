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
#ifndef CSOUNDFILE_H
#define CSOUNDFILE_H
#include "ISound.h"
#include <sndfile.h>
#include <consts.h>
#ifdef WITH_SAMPLERATE
#include <samplerate.h>
#endif
#include "CSoundStream.h"


class CSoundFile : public ISound
{
    Q_OBJECT
public:
    explicit CSoundFile(QObject *parent = 0);
    ~CSoundFile();
    bool Load(QString &fileName);
    bool Read(int16_t *data, int offset);
    bool ReadOverSample(int16_t *data, int offset, double ratio);
    void setRunning(bool value) override;
    void DecodeBuffer(int16_t *buffer, int size);
    void Record(QString &filename, bool start);
    void Play(int16_t *buffer, int size);
public slots:
    void setChannel(uint value);

private:
    SNDFILE*	pFile;
    int channels;
    int samplerate;
    sf_count_t frames;
    int16_t *inputbuffer;
    void run();
    void terminate();
    bool running;
    bool loop;
    int blankCount;
    int timing;
    CSoundStream *soundStream;
    QThread *audioEncode;
#ifdef WITH_SAMPLERATE
    float *inputbufferf;
    float *outputbufferf;
    // Sample rate conversion
    SRC_STATE* converter;
    SRC_DATA dataconv;
#endif

};

#endif // CSOUNDFILE_H
