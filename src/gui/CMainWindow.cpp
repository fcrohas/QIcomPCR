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

#include "CMainWindow.h"
#include "ui_CMainWindow.h"
#include <QScrollBar>
#include "CDevicePCR2500.h"
#include "version.h"

#include <QtGui>
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32
#include <io.h>
#define sleep Sleep
#endif

#include "config.h"

CMainWindow *theMainWindow = 0;

CMainWindow::CMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
    ,sound(NULL),
    bandscopeActivate(-1)

{
    theMainWindow = this;
    ui->setupUi(this);

    cmd = new CCommand(this);
    dbgWin = new CDebugWindow(this,ui);
    demodulator = new CDemodulator(this);
    remote = new CRemoteControl(this);
    settings =new CSettings(this);

    status = new CStatusWidget(this);
    display = new CDisplay(this);
#ifdef WITH_PULSEAUDIO
    sound  = new CPulseSound(this);
#endif
#ifdef WITH_PORTAUDIO
    sound  = new CPortAudio(this);
#endif
    // Spectrum widget
    mySpectrum = new CSpectrumWidget(this);
    ui->frequency1->addWidget(mySpectrum); // Widget channel 1

    myBandScope = new CBandScope(this);
    ui->bandscopeWidget->addWidget(myBandScope);

    // Add decoder view plotter
    myDecoder = new CSpectrumWidget(this);
    ui->DumpView->addWidget(myDecoder);
    // Decoder text
    ui->decoderText->setReadOnly(true);
    statusBar()->addPermanentWidget(status);

    // Set Squelch max
    ui->knobSquelch->setRange(0.0,255.0,1.0);
    ui->knobIF->setRange(0.0,255.0,1.0);
    ui->volume1->setRange(0.0,255.0,1.0);
    ui->layoutFrequencies->addWidget(display);
    // Add two default radio struct
    for (int i=0; i<MAX_RADIO; i++) {
        radioList.append(new CSettings::radio);
    }
#ifndef WIN32
    // Connect sound with demodulator
    sound->SetDemodulator(demodulator);
#endif
    connectSignals();
    mySpectrum->setAxis(0,16384,0,256);
    dock = addToolBar("File");
    dock->addAction(ui->actionLoad);
    dock->addAction(ui->actionQuit);

    restoreSettings();
}

CMainWindow::~CMainWindow()
{
    saveSettings();
    if (sound) {
        sound->setRunning(false);
        sound->terminate();
        delete sound;
        sound = NULL;
    }
}

void CMainWindow::connectSignals()
{
    connect(cmd,SIGNAL(dataChanged(QString)), this, SLOT(slotReceivedData(QString)));
    connect(ui->pushPower, SIGNAL(toggled(bool)), this, SLOT(powerOn(bool)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(cmd,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(ui->volume1, SIGNAL(valueChanged(double)), this,SLOT(slotVolume1(double)));
    //connect(ui->volume2, SIGNAL(valueChanged(double)), this,SLOT(slotVolume2(double)));
    connect(ui->knobSquelch,SIGNAL(valueChanged(double)), this, SLOT(slotSquelch(double)));
    connect(ui->knobIF,SIGNAL(valueChanged(double)), this, SLOT(slotIF(double)));
    connect(ui->pushNoiseBlanker,SIGNAL(toggled(bool)),this,SLOT(slotNoiseBlanker(bool)));
    connect(ui->pushAGC,SIGNAL(toggled(bool)),this,SLOT(slotAGC(bool)));
    connect(ui->pushVSC,SIGNAL(toggled(bool)),this,SLOT(slotVSC(bool)));


    // Connect filters
    connect(ui->filterGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotFilter(int)));
    // Connect Moudlation mode
    connect(ui->modeGroup,SIGNAL(buttonClicked(int)),this, SLOT(slotModulation(int)));

    // Frequency
    connect( display, SIGNAL(frequencyChanged(QString)), this,SLOT(slotFrequency(QString)));
    connect( myBandScope, SIGNAL(frequencyChanged(QString)), this, SLOT(slotFrequency(QString)));
    // Connect spectrum widget
    connect(demodulator,SIGNAL(sigRawSamples(double*,double*,int)),mySpectrum,SLOT(slotRawSamples(double*,double*,int)));

    // Connect Demodulator to debug windows
    connect(demodulator,SIGNAL(sendData(QString)),this,SLOT(slotDemodulatorData(QString)));

    // Set threshold
    connect(ui->threshold, SIGNAL(valueChanged(int)), demodulator, SLOT(slotThreshold(int)));
    connect(ui->correlationLength,SIGNAL(valueChanged(int)), demodulator, SLOT(slotSetCorrelationLength(int)));

    // Connect Decoder
    connect(ui->decoderList, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDecoderChange(int)));

    // Channel change
    connect(ui->channel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChannelChange(int)));

    // Connect Scope type
    connect(ui->cbPlotterType , SIGNAL(currentIndexChanged(int)),this, SLOT(slotScopeChanged(int)));
    connect(ui->refreshRate, SIGNAL(valueChanged(int)), this, SLOT(slotRefreshRate(int)));
    connect(ui->cbWindow, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotWindowFunction(QString)));

#ifndef WIN32
    connect(ui->pushSwitchSound,SIGNAL(clicked(bool)),this,SLOT(slotSwitchSound(bool)));
#endif
    // Band Scope
    connect(ui->pushBandscope,SIGNAL(clicked(bool)),this,SLOT(slotBandScope(bool)));
    connect(ui->cbBandwidth, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandScopeWidth(int)));
    connect(ui->cbStepsize, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandScopeStep(int)));

    // Step size change
    connect(ui->pushStepUp,SIGNAL(clicked()), this, SLOT(slotStepUp()));
    connect(ui->pushStepDown,SIGNAL(clicked()), this, SLOT(slotStepDown()));

    // Connect remote to event
    connect(remote,SIGNAL(sigAutomaticGainControl(bool)), cmd, SLOT(setAutomaticGainControl(bool)));
    connect(remote,SIGNAL(sigNoiseBlanker(bool)), cmd, SLOT(setNoiseBlanker(bool)));
    connect(remote,SIGNAL(sigSoundMute(bool)), cmd, SLOT(setSoundMute(bool)));
    connect(remote,SIGNAL(sigVoiceControl(bool)), cmd, SLOT(setVoiceControl(bool)));
    connect(remote,SIGNAL(sigFilter(uint)), cmd, SLOT(setFilter(uint)));
    connect(remote,SIGNAL(sigFrequency(uint)), cmd, SLOT(setFrequency(uint)));
    connect(remote,SIGNAL(sigIFShift(uint)), cmd, SLOT(setIFShift(uint)));
    connect(remote,SIGNAL(sigPower(bool)), cmd, SLOT(setPower(bool)));
    connect(remote,SIGNAL(sigRadio(uint)), cmd, SLOT(setRadio(uint)));
    connect(remote,SIGNAL(sigRadioMode(uint)), cmd, SLOT(setRadioMode(uint)));
    connect(remote,SIGNAL(sigSoundVolume(uint)), cmd, SLOT(setSoundVolume(uint)));
    connect(remote,SIGNAL(sigFrequency(uint)), cmd, SLOT(setFrequency(uint)));
    connect(remote,SIGNAL(sigModulation(uint)), cmd, SLOT(setModulation(uint)));
    connect(remote,SIGNAL(sigSquelch(uint)), cmd, SLOT(setSquelch(uint)));
    connect(remote,SIGNAL(sigChannel(int)), cmd, SLOT(slotChannelChange(int)));
    connect(remote,SIGNAL(sigDecoder(int)), cmd, SLOT(slotDecoderChange(int)));
    connect(remote,SIGNAL(sigRadio(uint)), cmd, SLOT(setRadio(uint)));
    connect(remote,SIGNAL(sigInitialize(bool)), this, SLOT(powerOn(bool)));
    connect(demodulator,SIGNAL(sigRawSamples(double*,double*,int)), remote, SLOT(controledRate(double*,double*,int)));
    // Connect load file
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(slotLoadFile()));
    connect(ui->pushStopPlay, SIGNAL(clicked(bool)), this, SLOT(slotStopPlay(bool)));
    connect(ui->pushRecord, SIGNAL(clicked(bool)), this, SLOT(slotRecordAudio(bool)));
    connect(display, SIGNAL(radioChanged(int)), this, SLOT(slotRadioClicked(int)));
    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(slotSettings()));
}

void CMainWindow::restoreSettings()
{
    // restore radio saved values
    for (int i=0; i<MAX_RADIO; i++) {
        CSettings::radio radio = settings->getRadio(i);
        radioList[i]->frequency = radio.frequency;
        radioList[i]->step = radio.step;
        radioList[i]->IF = radio.IF;
        radioList[i]->squelch = radio.squelch;
        radioList[i]->mode = radio.mode;
        radioList[i]->filter = radio.filter;
        radioList[i]->nb = radio.nb;
        radioList[i]->agc = radio.agc;
        radioList[i]->vsc = radio.vsc;

        // Set it on UI
        // Display only
        display->setRadio(i);
        display->setStepFromValue(radio.step);
        // other device
        cmd->setRadio(i);
        slotFrequency(QString("%1").arg(radio.frequency));
        slotIF(radio.IF);
        slotSquelch(radio.squelch);
        slotFilter(radio.filter);
        slotModulation(radio.mode);
        slotNoiseBlanker(radio.nb);
        slotVSC(radio.vsc);
        slotAGC(radio.agc);
        slotVolume1(radio.volume);
    }
}

void CMainWindow::saveSettings()
{
    for (int i=0; i<MAX_RADIO; i++) {
        radioList[i]->step = display->getStep(i);
        settings->setRadio(i,radioList[i]);
    }
}

void CMainWindow::powerOn(bool value)
{
    if (value) {
        if (cmd->Open()) {
            cmd->Initialize();
        } else ui->pushPower->setChecked(false);
        // Radio is powered on need to restoreprevious settings now
        restoreSettings();
    } else {
        cmd->Close();
        // Radio is power off save settings now
        saveSettings();
    }
    status->setState(cmd->getPower());
}

void CMainWindow::slotSendData(QString &data)
{
    cmd->write(data);
}

void CMainWindow::slotUpdateStatus()
{
    QString data("Data sent %1 bytes and received %2 bytes");
    status->slotUpdate(data.arg(cmd->getSendCount()).arg(cmd->getReadCount()));
}

void CMainWindow::slotReceivedData(QString data)
{
    bool found = false;
    // Signal antenne 1
    if (data.contains("I1")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I1")+2,2).toUInt(&ok,16);
        if ((ok) /* && (ui->radio1->isChecked())*/) {
            display->setSignal(0,value);
            remote->sendData(QString("@SA%1@").arg(value));
        }
        found = true;
    }
    // Signal antenne 2
    if (data.contains("I5")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I5")+2,2).toUInt(&ok,16);
        if ((ok) /* && (ui->radio2->isChecked())*/) {
            display->setSignal(1,value);
            remote->sendData(QString("@SB%1@").arg(value));
        }
        found = true;
    }
    if (data.contains("H100")) {
        statusBar()->showMessage(tr("Offline"));
        status->setState(false);
        remote->sendData(QString("@PWROFF@"));
        found = true;
    }
    if (data.contains("H101")) {
        statusBar()->showMessage(tr("Online"));
        status->setState(true);
        remote->sendData(QString("@PWRON@"));
        found = true;
    }
    if (data.contains("NE")) {
        myBandScope->setSamples(data);
        found = true;
    }
    if (!found) {
        dbgWin->slotDebugSerial(data);
        remote->sendData(QString("@DBG\r\n%1@").arg(data));
    }


    // Update status bar
    QString info("Data sent %1 %3bytes and received %2 %4bytes");
    int received = cmd->getReadCount();
    int sent       = cmd->getSendCount();
    QString receiveUnit("");
    QString sentUnit("");
    if (received > 9999)   { received = received / 1000.0; receiveUnit = "k"; }
    if (received > 999999) { received = received / 100000.0; receiveUnit = "M"; }

    if (sent > 9999)   { received = received / 1000.0; sentUnit = "k"; }
    if (sent > 999999) { received = received / 100000.0; sentUnit = "M";}


    status->slotUpdate(info.arg(sent).arg(received).arg(sentUnit).arg(receiveUnit));

}

void CMainWindow::slotVolume1(double value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setSoundVolume(value);
    ui->volume1->setValue(value);
    radioList[current]->volume = value;
    // Save to radio struct
}

void CMainWindow::slotSquelch(double value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setSquelch(value);
    ui->knobSquelch->setValue(value);
    // save to radio
    radioList[current]->squelch = value;
}

void CMainWindow::slotIF(double value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setIFShift(value);
    display->setIF(value);
    ui->knobIF->setValue(value);
    // save to radio
    radioList[current]->IF = value;
}

void CMainWindow::slotFrequency(QString value)
{
    if (value != "") {
        int current = display->getRadio();
        cmd->setRadio(current);
        cmd->setFrequency(value.toInt());

        // Set bandscope central frequency only on corerct entry
        if (current == bandscopeActivate)
            myBandScope->setCentralFrequency(value.toInt());

        display->setFrequency(value.toInt());
        // save to radio
        radioList[current]->frequency = value.replace(".","").toInt();
    }
}

void CMainWindow::slotStepUp()
{
    display->StepUp();
}

void CMainWindow::slotStepDown()
{
    display->StepDown();
}

void CMainWindow::slotFilter(int filter)
{
    // Filter come from UI value
    if (filter < 0) {
        filter = abs(filter)-2;
    }

    qDebug() << "Filter " << filter;
    // Convert buttongroup to enum value
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setFilter((CCommand::filter)filter);
    display->setFilter((CCommand::filter)filter);
    // save to radio
    radioList[current]->filter = (CCommand::filter)filter;
}

void CMainWindow::slotModulation(int mode)
{
    if(mode < 0) {
        mode = abs(mode)-2;
    }
    qDebug() << "Modulation " << mode;
    // Convert buttongroup to enum value
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setModulation((CCommand::mode)mode);
    display->setMode((CCommand::mode)mode);
    // save to radio
    radioList[current]->mode = (CCommand::filter)mode;
}

void CMainWindow::slotRadioClicked(int value)
{
    // Update knob settings
    ui->knobSquelch->setValue(radioList[value]->squelch);
    ui->knobIF->setValue(radioList[value]->IF);
    ui->pushNoiseBlanker->setChecked(radioList[value]->nb);
    ui->pushVSC->setChecked(radioList[value]->vsc);
    ui->pushAGC->setChecked(radioList[value]->agc);
    ui->volume1->setValue(radioList[value]->volume);
}

void CMainWindow::slotSwitchSound(bool value)
{
#if defined(WITH_SNDFILE) || defined(WITH_PORTAUDIO) || defined(WITH_PULSEAUDIO)
    if (value == true) {
        if (sound == NULL) {
#ifdef WITH_PULSEAUDIO
            sound  = new CPulseSound(this);
#endif
#ifdef WITH_PORTAUDIO
            sound  = new CPortAudio(this);
#endif
        }
        // Restore sound settings
        CSettings::global *params = new CSettings::global();
        settings->getGlobal(params);
        #ifndef WIN32
            qDebug() << "sound input " << params->inputDevice;
            sound->selectInputDevice(params->inputDevice);
        #endif
        #ifndef WIN32
            qDebug() << "sound output " << params->outputDevice;
            sound->selectOutputDevice(params->outputDevice);
        #endif
        delete params;
        sound->SetDemodulator(demodulator);

        demodulator->initBuffer(32768);
        sound->setRunning(true);
        sound->start();
    }
    else {
        sound->setRunning(false);
        sound->terminate();
        sound->wait();
        delete sound;
        sound = NULL;
    }
#endif
}

void CMainWindow::slotNoiseBlanker(bool value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setNoiseBlanker(value);
    ui->pushNoiseBlanker->setChecked(value);
    // save to radio
    radioList[current]->nb = value;
}

void CMainWindow::slotAGC(bool value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setAutomaticGainControl(value);
    ui->pushAGC->setChecked(value);
    // save to radio
    radioList[current]->agc = value;
}

void CMainWindow::slotVSC(bool value)
{
    int current = display->getRadio();
    cmd->setRadio(current);
    cmd->setVoiceControl(value);
    ui->pushVSC->setChecked(value);
    // save to radio
    radioList[current]->vsc = value;
}

void CMainWindow::slotDemodulatorData(QString data)
{
    ui->decoderText->setPlainText(ui->decoderText->toPlainText()+data);
    //ui->decoderText->append(data);
    QScrollBar *sb = ui->decoderText->verticalScrollBar();
    sb->setValue(sb->maximum());
    remote->sendData(QString("@DEM\t%1@").arg(data));
}

void CMainWindow::slotDecoderChange(int value)
{
    int channel = ui->channel->currentIndex();
    qDebug() << "CMainWindow::slotDecoderChange(" << value << ")";
    demodulator->slotSetDemodulator(value,channel,16384);
    // Connect Demodulator to debug windows
    connect(demodulator->getDemodulatorFromChannel(channel),SIGNAL(dumpData(double*,double*,int)),myDecoder,SLOT(slotRawSamples(double*,double*,int)));
    connect(mySpectrum, SIGNAL(frequency(double)), demodulator->getDemodulatorFromChannel(channel), SLOT(slotFrequency(double)));
    connect(mySpectrum, SIGNAL(bandwidth(double)), demodulator->getDemodulatorFromChannel(channel), SLOT(slotBandwidth(double)));
    connect(remote, SIGNAL(sigSelectFrequency(double)), demodulator->getDemodulatorFromChannel(channel), SLOT(slotFrequency(double)));
    connect(remote, SIGNAL(sigSelectBandwidth(double)), demodulator->getDemodulatorFromChannel(channel), SLOT(slotBandwidth(double)));
    connect(myDecoder, SIGNAL(bandwidth(double)), this, SLOT(slotThreshold(double)));
    //myDecoder->setScaleType(CSpectrumWidget::eTime);
    if (value == 4) {
        myDecoder->setAxis(0,512,-30.0,30.0);
        myDecoder->setPickerType(CSpectrumWidget::eThresholdPicker);
        mySpectrum->setPickerType(CSpectrumWidget::eRttyPicker);
    }
    else if (value==3) {
        myDecoder->setAxis(0,512,0.0,15.0);
        myDecoder->setPickerType(CSpectrumWidget::eThresholdPicker);
        mySpectrum->setPickerType(CSpectrumWidget::eCwPicker);
    } else {
        myDecoder->setAxis(0,512,-15.0,15.0);
        myDecoder->setPickerType(CSpectrumWidget::eThresholdPicker);
        mySpectrum->setPickerType(CSpectrumWidget::eNoPicker);
    }
}

void CMainWindow::slotChannelChange(int value)
{
    qDebug() << "CMainWindow::slotChannelChange(" << value << ")";
    //demodulator->slotSetDemodulator(ui->decoderList->currentIndex(), value, 16384);
}

void CMainWindow::slotScopeChanged(int value)
{
    qDebug() << "cb changed =" << value;
    mySpectrum->setPlotterType((CSpectrumWidget::ePlotter)value);
    if (value == 1) {
        demodulator->setScopeType(1);
        mySpectrum->setAxis(0,FFTSIZE,0,50);
    } else
    if (value == 2) {
        demodulator->setScopeType(1);
        mySpectrum->setAxis(0,FFTSIZE,0,WATERFALL_MAX);
    }
    else {
        demodulator->setScopeType(0);
        mySpectrum->setAxis(0,1024,0,256);
    }
}

void CMainWindow::slotRemoteData(QString &data)
{

}

void CMainWindow::slotLoadFile()
{
    // File dialog chooser
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Sound"), QDir::homePath(), tr("Sound Files (*.wav *.flac *.au *.voc *.ogg)"));
#ifdef WITH_SNDFILE
    if (!fileName.isEmpty()) {
        // Close sound card reader
        if (sound) {
            sound->terminate();
            delete sound;
        }
        // Create new sound reader from file
        sound = new CSoundFile(this);
        sound->Load(fileName);
        sound->SetDemodulator(demodulator);
        sound->setRunning(true);
    }
#endif
    slotSwitchSound(true);
}

void CMainWindow::slotStopPlay(bool value)
{
    if (value == true) {
#ifdef WITH_PULSEAUDIO
        sound  = new CPulseSound(this);
#endif
#ifdef WITH_PORTAUDIO
        sound  = new CPortAudio(this);
#endif
        sound->SetDemodulator(demodulator);
    } else {
        sound->setRunning(false);
        sleep(5);
        sound->terminate();
        delete sound;
        sound = NULL;
    }
}

void CMainWindow::slotWindowFunction(QString value)
{
    if(value == "Blackman-Harris") demodulator->slotChangeWindowFunction(CFFT::BlackmanHarris);
    if(value == "Blackman") demodulator->slotChangeWindowFunction(CFFT::Blackman);
    if(value == "Hann") demodulator->slotChangeWindowFunction(CFFT::Hann);
    if(value == "Hamming") demodulator->slotChangeWindowFunction(CFFT::Hamming);
    if(value == "Rectangle") demodulator->slotChangeWindowFunction(CFFT::Rectangle);
}

void CMainWindow::slotRecordAudio(bool value)
{
    if (sound != NULL)  {
        if (value == true) {
            // File dialog chooser
            QString fileName = QFileDialog::getSaveFileName(this,
                 tr("Record Sound"), QDir::homePath(), tr("Sound Files (*.wav)"));
            if (!fileName.isEmpty()) {
                fileName = fileName.append(".wav");
                sound->Record(fileName, true);
            }
        } else {
            QString fileName("");
            sound->Record(fileName, false);
        }
    }
}

void CMainWindow::slotRefreshRate(int value)
{
    mySpectrum->setRefreshRate(value);
}

void CMainWindow::slotBandScope(bool value)
{

    if (value) {
        bandscopeActivate = display->getRadio();
        cmd->setBandScope((CCommand::radioA)bandscopeActivate,04,true);
    } else {
        bandscopeActivate = -1;
        cmd->setBandScope(CCommand::eRadio1,04,false);
    }
}

void CMainWindow::slotBandScopeWidth(int value)
{
    myBandScope->setBandWidth(bandwidth[value]);
    cmd->setBandScopeWidth(bandwidth[value]);
}

void CMainWindow::slotBandScopeStep(int value)
{
    myBandScope->setStep(stepsize[value]);
    cmd->setBandScopeStep(stepsize[value]);
}

void CMainWindow::slotSettings()
{
    CDlgSettings *dlgparam = new CDlgSettings(this, settings);
    dlgparam->addInputSoundDevices(sound->getDeviceList());
    dlgparam->addOutputSoundDevices(sound->getDeviceList());
    dlgparam->initialize();
    dlgparam->exec();
    delete dlgparam;
}

void CMainWindow::slotThreshold(double threshold)
{
    int channel = ui->channel->currentIndex();
    demodulator->getDemodulatorFromChannel(channel)->setThreshold(threshold);
}
