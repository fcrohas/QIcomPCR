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
#include "CSettings.h"

CSettings::CSettings(QObject *parent) :
    QSettings("Lilisoft.com","QIcomPcr",parent)
{

}

CSettings::radio_t CSettings::getRadio(int value)
{
    radio_t current;
    beginGroup(QString("Radio/Antenna%1").arg(value));
    current.frequency = this->value("Frequency", 106500000).toInt(); // RTS FM
    current.filter = this->value("Filter", CCommand::e230k).toInt(); // WFM Filter
    current.step = this->value("Step", 50000).toInt(); // WFM is 50Khz step
    current.IF = this->value("IF", 128).toInt(); // Middle by default
    current.squelch = this->value("Squelch", 0).toInt(); // Middle by default
    current.nb = this->value("NoiseBlanker", CCommand::eNBOff).toBool();
    current.agc = this->value("AGC", CCommand::eAgcOff).toBool();
    current.vsc = this->value("VSC", CCommand::eVSCOff).toBool();
    current.mode = this->value("Mode", CCommand::eWFM).toInt();
    current.volume = this->value("Volume",0).toInt();
    endGroup();
    return current;
}

void CSettings::setRadio(int radionum,CSettings::radio_t *value)
{
    beginGroup(QString("Radio/Antenna%1").arg(radionum));
    this->setValue("Frequency",value->frequency);
    this->setValue("Filter",value->filter);
    this->setValue("Step",value->step);
    this->setValue("IF", value->IF);
    this->setValue("Squelch",value->squelch);
    this->setValue("NoiseBlanker",value->nb);
    this->setValue("AGC",value->agc);
    this->setValue("VSC",value->vsc);
    this->setValue("Mode",value->mode);
    this->setValue("Volume",value->volume);
    endGroup();
}

CSettings::sound_t CSettings::getSound(int value)
{
    sound_t current;
    beginGroup(QString("Sound/Channel%1").arg(value));
    current.volume = this->value("Volume", 15).toInt(); // Volume
    current.soundcard = this->value("SoundOnPC", false).toBool(); // Volume
    current.activated = this->value("Activated", true).toBool(); // Volume
    endGroup();
    return current;
}

void CSettings::setSound(int radionum, sound_t *value)
{
    beginGroup(QString("Sound/Channel%1").arg(radionum));
    this->setValue("Volume",value->volume);
    this->setValue("SoundOnPC",value->soundcard);
    this->setValue("Activated",value->activated);
    endGroup();

}

void CSettings::getGlobal(global *value)
{
    beginGroup("Global");
    value->inputDevice = this->value("InputDevice","default").toString();
    value->outputDevice = this->value("OutputDevice","default").toString();
    value->samplerate = this->value("SampleRate",22050).toInt();
    value->fftSize = this->value("FFTSize").toInt();
    value->soundBufferSize = this->value("BufferSize",512).toInt();
    endGroup();
}

void CSettings::setGlobal(global *value)
{
    beginGroup("Global");
    this->setValue("InputDevice",value->inputDevice);
    this->setValue("OutputDevice",value->outputDevice);
    this->setValue("SampleRate",value->samplerate);
    this->setValue("BufferSize",value->soundBufferSize);
    this->setValue("FFTSize",value->fftSize);
    endGroup();
}
