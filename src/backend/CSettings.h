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
#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QObject>
#include <QSettings>
#include <CCommand.h>

class CSettings : public QSettings
{
    Q_OBJECT
public:
    explicit CSettings(QObject *parent = 0);
    void reload();
    void save();

    struct radio_t {
        int frequency;
        int step;
        int filter;
        int IF;
        int squelch;
        int mode;
        bool agc;
        bool vsc;
        bool nb;
        int volume;
    };

    struct sound_t {
        int volume;
        bool soundcard;
        bool activated;
        bool mute;
    };

    struct global {
        QString inputDevice;
        QString outputDevice;
        int samplerate;
        int soundBufferSize;
        int fftSize;
    };

    radio_t getRadio(int value);
    void setRadio(int radionum, radio_t *value);

    sound_t getSound(int value);
    void setSound(int radionum, sound_t *value);

    void getGlobal(global *value);
    void setGlobal(global *value);
signals:
    
public slots:

private:
    void initialize();

};

#endif // CSETTINGS_H
