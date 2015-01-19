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
#include "version.h"

#include <QtWidgets>
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
{
    theMainWindow = this;
    ui->setupUi(this);
    // Build backend controler
    backend = new CBackend();

    // Build widgets UI
    dbgWin = new CDebugWindow(this,ui);
    status = new CStatusWidget(this);
    display = new CDisplay(this);
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
    ui->knobSquelch->setScale(0.0,255.0);
    ui->knobIF->setScale(0.0,255.0);
    ui->volume1->setScale(0.0,255.0);
    ui->layoutFrequencies->addWidget(display);
    // Add two default radio struct
    for (int i=0; i<MAX_RADIO; i++) {
        radioList.append(new CSettings::radio_t);
    }
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
}

void CMainWindow::connectSignals()
{
    //connect(cmd,SIGNAL(dataChanged(QString)), this, SLOT(slotReceivedData(QString)));
    connect(ui->pushPower, SIGNAL(toggled(bool)), this, SLOT(powerOn(bool)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    //connect(cmd,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
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
    /*// Connect spectrum widget
    connect(decoder,SIGNAL(sigRawSamples(double*,double*,int)),mySpectrum,SLOT(slotRawSamples(double*,double*,int)));

    // Connect Demodulator to debug windows
    connect(decoder,SIGNAL(sendData(QString)),this,SLOT(slotDemodulatorData(QString)));

    // Set threshold
    connect(ui->threshold, SIGNAL(valueChanged(int)), decoder, SLOT(slotThreshold(int)));
    connect(ui->correlationLength,SIGNAL(valueChanged(int)), decoder, SLOT(slotSetCorrelationLength(int)));
    */
    // Connect Decoder
    connect(ui->decoderList, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDecoderChange(int)));

    // Channel change
    connect(ui->channel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChannelChange(int)));

    // Connect Scope type
    connect(ui->cbPlotterType , SIGNAL(currentIndexChanged(int)),this, SLOT(slotScopeChanged(int)));
    connect(ui->refreshRate, SIGNAL(valueChanged(int)), this, SLOT(slotRefreshRate(int)));
    connect(ui->cbWindow, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotWindowFunction(QString)));

    connect(ui->pushSwitchSound,SIGNAL(clicked(bool)),this,SLOT(slotSwitchSound(bool)));
    // Band Scope
    connect(ui->pushBandscope,SIGNAL(clicked(bool)),this,SLOT(slotBandScope(bool)));
    connect(ui->cbBandwidth, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandScopeWidth(int)));
    connect(ui->cbStepsize, SIGNAL(currentIndexChanged(int)), this, SLOT(slotBandScopeStep(int)));

    // Step size change
    connect(ui->pushStepUp,SIGNAL(clicked()), this, SLOT(slotStepUp()));
    connect(ui->pushStepDown,SIGNAL(clicked()), this, SLOT(slotStepDown()));
/*
    connect(remote,SIGNAL(sigChannel(int)), this, SLOT(slotChannelChange(int)));
    connect(remote,SIGNAL(sigDecoder(int)), this, SLOT(slotDecoderChange(int)));
    connect(remote,SIGNAL(sigInitialize(bool)), this, SLOT(powerOn(bool)));
    connect(remote,SIGNAL(sigBandScope(bool)), this, SLOT(slotBandScope(bool)));
    connect(remote,SIGNAL(sigBandScopeWidth(int)), this, SLOT(slotBandScopeWidth(int)));
    connect(remote,SIGNAL(sigBandScopeStep(int)), this, SLOT(slotBandScopeStep(int)));
    connect(decoder,SIGNAL(sigRawSamples(double*,double*,int)), remote, SLOT(controledRate(double*,double*,int)));
*/
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
/*
        CSettings::radio_t radio = settings->getRadio(i);
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
*/
    }
}

void CMainWindow::saveSettings()
{
    for (int i=0; i<MAX_RADIO; i++) {
        radioList[i]->step = display->getStep(i);
        //settings->setRadio(i,radioList[i]);
    }
}

void CMainWindow::powerOn(bool value)
{
    if (value) {
            backend->setPower(value);
        } else ui->pushPower->setChecked(false);
        // Radio is powered on need to restoreprevious settings now
        restoreSettings();
    //status->setState(cmd->getPower());
}

void CMainWindow::slotSendData(QString &data)
{
    //cmd->write(data);
}

void CMainWindow::slotUpdateStatus()
{
    //QString data("Data sent %1 bytes and received %2 bytes");
    //status->slotUpdate(data.arg(cmd->getSendCount()).arg(cmd->getReadCount()));
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
        remote->sendData(QString("@BDS%1@").arg(data.replace("NE1","")));
        found = true;
    }
    if (!found) {
        dbgWin->slotDebugSerial(data);
        remote->sendData(QString("@DBG\r\n%1@").arg(data));
    }


    // Update status bar
    QString info("Data sent %1 %3bytes and received %2 %4bytes");
    int received = 0; //cmd->getReadCount();
    int sent       = 0; //cmd->getSendCount();
    QString receiveUnit("");
    QString sentUnit("");
    if (received > 9999)   { received = received / 1000.0; receiveUnit = "k"; }
    if (received > 9999) { received = received / 1000.0; receiveUnit = "M"; }
    if (received > 9999) { received = received / 1000.0; receiveUnit = "G"; }

    if (sent > 9999)   { sent = sent / 1000.0; sentUnit = "k"; }
    if (sent > 9999) { sent = sent / 1000.0; sentUnit = "M";}
    if (sent > 9999) { sent = sent / 1000.0; sentUnit = "G";}


    status->slotUpdate(info.arg(sent).arg(received).arg(sentUnit).arg(receiveUnit));

}

void CMainWindow::slotVolume1(double value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setSoundVolume(value);
    ui->volume1->setValue(value);
    radioList[current]->volume = value;
    // Save to radio struct
}

void CMainWindow::slotSquelch(double value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setSquelch(value);
    ui->knobSquelch->setValue(value);
    // save to radio
    radioList[current]->squelch = value;
}

void CMainWindow::slotIF(double value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setIFShift(value);
    display->setIF(value);
    ui->knobIF->setValue(value);
    // save to radio
    radioList[current]->IF = value;
}

void CMainWindow::slotFrequency(QString value)
{
    if (value != "") {
        int current = display->getRadio();
        //cmd->setRadio(current);
        //cmd->setFrequency(value.toInt());

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
    //cmd->setRadio(current);
    //cmd->setFilter((CCommand::filter)filter);
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
    //cmd->setRadio(current);
    //cmd->setModulation((CCommand::mode)mode);
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

}

void CMainWindow::slotNoiseBlanker(bool value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setNoiseBlanker(value);
    ui->pushNoiseBlanker->setChecked(value);
    // save to radio
    radioList[current]->nb = value;
}

void CMainWindow::slotAGC(bool value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setAutomaticGainControl(value);
    ui->pushAGC->setChecked(value);
    // save to radio
    radioList[current]->agc = value;
}

void CMainWindow::slotVSC(bool value)
{
    int current = display->getRadio();
    //cmd->setRadio(current);
    //cmd->setVoiceControl(value);
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
    decoder->slotSetDemodulator(value,channel,16384);
    // Connect Demodulator to debug windows
    connect(decoder->getDemodulatorFromChannel(channel),SIGNAL(dumpData(double*,double*,int)),myDecoder,SLOT(slotRawSamples(double*,double*,int)));
    connect(mySpectrum, SIGNAL(frequency(double)), decoder->getDemodulatorFromChannel(channel), SLOT(slotFrequency(double)));
    connect(myDecoder, SIGNAL(threshold(double)), decoder->getDemodulatorFromChannel(channel), SLOT(setThreshold(double)));
    connect(mySpectrum, SIGNAL(bandwidth(double)), decoder->getDemodulatorFromChannel(channel), SLOT(slotBandwidth(double)));
    connect(remote, SIGNAL(sigSelectFrequency(double)), decoder->getDemodulatorFromChannel(channel), SLOT(slotFrequency(double)));
    connect(remote, SIGNAL(sigSelectBandwidth(double)), decoder->getDemodulatorFromChannel(channel), SLOT(slotBandwidth(double)));
    //connect(myDecoder, SIGNAL(threshold(double)), this, SLOT(slotThreshold(double)));
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
    decoder->slotSetChannel(value);
    ui->channel->setCurrentIndex (value);
    //demodulator->slotSetDemodulator(ui->decoderList->currentIndex(), value, 16384);
}

void CMainWindow::slotScopeChanged(int value)
{
    qDebug() << "cb changed =" << value;
    mySpectrum->setPlotterType((CSpectrumWidget::ePlotter)value);
    if (value == 1) {
        decoder->setScopeType(1);
        mySpectrum->setAxis(0,FFTSIZE,0,50);
    } else
    if (value == 2) {
        decoder->setScopeType(1);
        mySpectrum->setAxis(0,FFTSIZE,0,WATERFALL_MAX);
    }
    else {
        decoder->setScopeType(0);
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
#ifdef WITH_SNDFILE1
    if (!fileName.isEmpty()) {
        // Close sound card reader
        if (sound) {
            sound->terminate();
            delete sound;
        }
        // Create new sound reader from file
        sound = new CSoundFile(this);
        sound->Load(fileName);
        sound->SetDecoder(decoder);
        sound->setRunning(true);
        connect(remote,SIGNAL(sigRadio(uint)), sound, SLOT(setChannel(uint)));
    }
#endif
    slotSwitchSound(true);
}

void CMainWindow::slotStopPlay(bool value)
{

}

void CMainWindow::slotWindowFunction(QString value)
{
    if(value == "Blackman-Harris") decoder->slotChangeWindowFunction(CFFT::BlackmanHarris);
    if(value == "Blackman") decoder->slotChangeWindowFunction(CFFT::Blackman);
    if(value == "Hann") decoder->slotChangeWindowFunction(CFFT::Hann);
    if(value == "Hamming") decoder->slotChangeWindowFunction(CFFT::Hamming);
    if(value == "Rectangle") decoder->slotChangeWindowFunction(CFFT::Rectangle);
}

void CMainWindow::slotRecordAudio(bool value)
{
    //if (sound != NULL)  {
        if (value == true) {
            // File dialog chooser
            QString fileName = QFileDialog::getSaveFileName(this,
                 tr("Record Sound"), QDir::homePath(), tr("Sound Files (*.wav)"));
            if (!fileName.isEmpty()) {
                fileName = fileName.append(".wav");
                //sound->Record(fileName, true);
            }
        } else {
            QString fileName("");
            //sound->Record(fileName, false);
        }
    //}
}

void CMainWindow::slotRefreshRate(int value)
{
    mySpectrum->setRefreshRate(value);
}

void CMainWindow::slotBandScope(bool value)
{

    if (value) {
        bandscopeActivate = display->getRadio();
        //cmd->setBandScope((CCommand::radioA)bandscopeActivate,04,true);
    } else {
        bandscopeActivate = -1;
        //cmd->setBandScope(CCommand::eRadio1,04,false);
    }
}

void CMainWindow::slotBandScopeWidth(int value)
{
    qDebug() << "Band Scope Width "<< value;
    myBandScope->setBandWidth(bandwidth[value]);
    //cmd->setBandScopeWidth(bandwidth[value]);
}

void CMainWindow::slotBandScopeStep(int value)
{
    qDebug() << "Band Scope Step "<< value;
    myBandScope->setStep(stepsize[value]);
    //cmd->setBandScopeStep(stepsize[value]);
}

void CMainWindow::slotSettings()
{
    /*
    CDlgSettings *dlgparam = new CDlgSettings(this, settings);
    dlgparam->addInputSoundDevices(sound->getDeviceList());
    dlgparam->addOutputSoundDevices(sound->getDeviceList());
    dlgparam->initialize();
    dlgparam->exec();
    delete dlgparam;
    */
}

void CMainWindow::slotThreshold(double threshold)
{
    int channel = ui->channel->currentIndex();
    decoder->getDemodulatorFromChannel(channel)->setThreshold(threshold);
}
