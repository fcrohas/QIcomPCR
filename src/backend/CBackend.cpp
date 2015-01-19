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

#include "CBackend.h"

CBackend::CBackend(QObject *parent) :
    QObject(parent)
{
    // Install sound device
    sound = CSound::Builder(CSound::ePortAudio);
    // Install Decoder
    decoders = new CDecoder(this);
    // Install Radio device
    cmd = new CCommand(this);
    // Remote control TCP/IP
    remote = new CRemoteControl(this);
    // Settings load/save
    settings =new CSettings(this);
    // Initialize step
    initializeRemote();
    initializeDecoders();
    initializeDevice();
    // Restore settings
    restoreSettings();
}

CBackend::~CBackend() {
    // Shutdown device
    setPower(false);
    // Save settings
    saveSettings();
    // Free
    delete sound;
    delete decoders;
    delete remote;
    delete settings;
    delete cmd;
}

void CBackend::initializeRemote() {
    // Connect remote to radio device event
    connect(remote,SIGNAL(sigRadio(CCommand::radio_t)), cmd, SLOT(setRadio(CCommand::radio_t)));
    // Connect remote to bandscope event
    connect(remote,SIGNAL(sigBandScope(CCommand::bandscope_t)), cmd, SLOT(setBandscope(CCommand::bandscope_t)));
    // Connect remote to decoder event
    connect(remote,SIGNAL(sigDecoder(CDecoder::decoder_t)), decoders, SLOT(setDecoder(CDecoder::decoder_t)));
    // Connect sound event
    connect(remote,SIGNAL(sigSoundMute(bool)), cmd, SLOT(setSoundMute(bool)));
    connect(remote,SIGNAL(sigSoundVolume(uint)), cmd, SLOT(setSoundVolume(uint)));
}

void CBackend::initializeDecoders() {
    // Send decoder event to remote control
    connect(decoders,SIGNAL(sigRawSamples(double*,double*,int)), remote, SLOT(controledRate(double*,double*,int)));
    // Connect Demodulator to debug windows
    connect(decoders,SIGNAL(sendData(QString)),this,SLOT(setDemodulatorData(QString)));
    // Connect spectrum widget
    connect(decoders,SIGNAL(sigRawSamples(double*,double*,int)),this,SLOT(slotRawSamples(double*,double*,int)));
}

void CBackend::initializeDevice() {
    connect(cmd,SIGNAL(dataChanged(status_t)), this, SLOT(statusChanged(CCommand::status_t)));
}

CCommand::radio_t CBackend::getRadioSettings(int radio) {
    return settings->getRadio(radio);
}

void CBackend::restoreSettings() {

}

void CBackend::saveSettings() {

}

bool CBackend::getPower() {
    return cmd->getPower();
}

void CBackend::setPower(bool value) {
    if (value) {
        if (cmd->Open()) {
            cmd->Initialize();
        }
        // Radio is powered on need to restoreprevious settings now
        restoreSettings();
    } else {
        cmd->Close();
        // Radio is power off save settings now
        saveSettings();
    }
}

// Bandscope properties
void CBackend::setBandscope(CCommand::bandscope_t scope) {
    this->bandscope = scope;
    cmd->setBandscope(scope);
}

CCommand::bandscope_t CBackend::getBandscope() {
    return this->bandscope;
}

// Decoder properties
void CBackend::setDecoder(CDecoder::decoder_t decoder) {
    this->decoder = decoder;
    decoders->setDecoder(decoder);
}

CDecoder::decoder_t CBackend::getDecoder() {
    return this->decoder;
}

// Radio properties
void CBackend::setRadio(CCommand::radio_t radio) {
    this->radio = radio;
    cmd->setRadio(radio);
}

CCommand::radio_t CBackend::getRadio() {
    return this->radio;
}

void CBackend::statusChanged(CCommand::status_t status) {
    emit sigStatus(status);
}
