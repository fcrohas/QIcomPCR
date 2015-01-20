/**********************************************************************************************
Copyright (C) 2013 Fabrice Crohas fcrohas@gmail.com

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

#ifndef CBACKEND_H
#define CBACKEND_H

#include <QObject>
#include <CSound.h>
#include <CDecoder.h>
#include <CCommand.h>
#include <CSettings.h>
#include <CRemoteControl.h>

class CBackend : public QObject
{
    Q_OBJECT
public:
    explicit CBackend(QObject *parent = 0);
    ~CBackend();
    void initializeRemote();
    void initializeDecoders();
    void initializeDevice();
    // Settings load/save
    CCommand::radio_t getRadioSettings(int radio);
    void restoreSettings();
    void saveSettings();
    // Decoder object manager
    CDecoder* getDecoder();
    // Sound object manager
    ISound* getSound();
    // Settings Manager
    CSettings* getSettings();

signals:
    void sigStatus(CCommand::status_t status);
    
public slots:
    // Power device
    void setPower(bool value);
    bool getPower();
    // Bandscope properties setter/getter
    void setBandscope(CCommand::bandscope_t scope);
    CCommand::bandscope_t getBandscope();
    // Decoder setter/getter
    void setDecoder(CDecoder::decoder_t decoder);
    // Radio setter/getter
    void setRadio(CCommand::radio_t radio);
    CCommand::radio_t getRadio();
    // Radio status
    void statusChanged(CCommand::status_t status);

private:
    // Sound control
    ISound *sound;
    // Decoders
    CDecoder *decoders;
    // Device control
    CCommand *cmd;
    // Settings class loader
    CSettings *settings;
    // Remote controler command
    CRemoteControl *remote;
    // radio struct
    CCommand::radio_t radio;
    // bandscope struct
    CCommand::bandscope_t bandscope;
    // decoder struct
    CDecoder::decoder_t decoder;

};

#endif // CBACKEND_H
