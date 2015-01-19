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
#ifndef CSOUND_H
#define CSOUND_H

#include <ISound.h>
#ifdef WITH_SNDFILE
#include <CSoundFile.h>
#endif
#ifdef WITH_PORTAUDIO
#include <CPortAudio.h>
#endif
#ifdef WITH_PULSEAUDIO
#include <CPulseSound.h>
#endif
#include <CSoundStream.h>

class CSound : public QObject
{
    Q_OBJECT
public:
    enum SoundType { ePulse=0, ePortAudio=1, eFile=2, eStream=3};
    static ISound *Builder(SoundType type);
signals:
    
public slots:
    
};

#endif // CSOUND_H
