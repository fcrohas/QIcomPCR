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
}

void CBackend::initializeDevice() {
    connect(cmd,SIGNAL(dataChanged(CCommand::status_t)), this, SLOT(statusChanged(CCommand::status_t)));
}

CCommand::radio_t CBackend::getRadioSettings(int radio) {
    return settings->getRadio(radio);
}

void CBackend::restoreSettings() {
    CSettings::global *params = new CSettings::global();
    settings->getGlobal(params);
    // Install sound device
    sound = CSound::Builder(CSound::ePortAudio);
    // Input device
    sound->selectInputDevice(params->inputDevice);
    qDebug() << "input device " << params->inputDevice << "\r\n";
    // output device
    sound->selectOutputDevice(params->outputDevice);
    qDebug() << "output device " << params->outputDevice << "\r\n";
    // decoders
    sound->SetDecoder(decoders);
    decoders->initBuffer(32768);
    cmd->setSoundDevice(sound);
    // Sound initialize
    sound->Initialize();
    sound->setRunning(true);
}

void CBackend::saveSettings() {
}

bool CBackend::getPower() {
    return cmd->getPower();
}

void CBackend::setPower(bool value) {
    if (value) {
        // Radio is powered on need to restoreprevious settings now
        restoreSettings();
        if (cmd->Open()) {
            cmd->Initialize();
        }
    } else {
        cmd->Close();
        // Radio is power off save settings now
        saveSettings();
    }
    radio.power = value;
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
    bool channelChanges = false;
    if (decoder.channel != this->decoder.channel) {
        // disconnect slot
        disconnect(remote,SIGNAL(sigSelectFrequency(double)));
        disconnect(remote,SIGNAL(sigSelectBandwidth(double)));
        channelChanges = true;
    }
    this->decoder = decoder;
    decoders->setDecoder(decoder);
    if (channelChanges) {
        // connect newer
        connect(remote, SIGNAL(sigSelectFrequency(double)), decoders->getDemodulatorFromChannel(decoder.channel), SLOT(slotFrequency(double)));
        connect(remote, SIGNAL(sigSelectBandwidth(double)), decoders->getDemodulatorFromChannel(decoder.channel), SLOT(slotBandwidth(double)));
    }
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

CDecoder* CBackend::getDecoder() {
    return decoders;
}

ISound* CBackend::getSound() {
    return sound;
}

CSettings* CBackend::getSettings() {
    return settings;
}
