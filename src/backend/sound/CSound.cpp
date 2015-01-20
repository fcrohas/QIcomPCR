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
#include "CSound.h"

ISound *CSound::Builder(SoundType type) {
    ISound *sound = NULL;
    // Build factory
    switch(type) {
#ifdef WITH_SNDFILE
        case eFile : return sound = new CSoundFile(); break;
#endif
#ifdef WITH_PORTAUDIO
    case ePortAudio : return sound = new CPortAudio(0,ISound::ePlay); break;
#endif
#ifdef WITH_PULSEAUDIO
        case ePulse : return new CPulseSound();  break;
#endif
        case eStream : return sound = new CSoundStream(); break;
    }
    return sound;
}
