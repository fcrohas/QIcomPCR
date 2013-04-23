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
{
    theMainWindow = this;
    ui->setupUi(this);

    cmd = new CCommand(this);
    dbgWin = new CDebugWindow(this,ui);
    demodulator = new CDemodulator(this);
    remote = new CRemoteControl(this);

    status = new CStatusWidget(this);
    lcd    = new CLcdWidget(this);
#ifdef WITH_PULSEAUDIO
    sound  = new CPulseSound(this);
#endif
#ifdef WITH_PORTAUDIO
    sound  = new CPortAudio(this);
#endif
    // Spectrum widget
    mySpectrum = new CSpectrumWidget(this);
    ui->frequency1->addWidget(mySpectrum); // Widget channel 1

    // Add decoder view plotter
    myDecoder = new CSpectrumWidget(this);
    ui->DumpView->addWidget(myDecoder);
    // Decoder text
    ui->decoderText->setReadOnly(true);
    statusBar()->addPermanentWidget(status);

    m_device = new CDevicePCR2500("/dev/ttyUSB0", "38400", this);

    // Set Squelch max
    ui->knobSquelch->setRange(0.0,255.0,1.0);
    ui->knobIF->setRange(0.0,255.0,1.0);

    connect(ui->pushPower, SIGNAL(clicked()), this, SLOT(powerOn()));
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(cmd,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(ui->volume1, SIGNAL(valueChanged(int)), this,SLOT(slotVolume1(int)));
    connect(ui->volume2, SIGNAL(valueChanged(int)), this,SLOT(slotVolume2(int)));
    connect(ui->knobSquelch,SIGNAL(valueChanged(double)), this, SLOT(slotSquelch(double)));
    connect(ui->knobIF,SIGNAL(valueChanged(double)), this, SLOT(slotIF(double)));
    connect(ui->pushNoiseBlanker,SIGNAL(toggled(bool)),this,SLOT(slotNoiseBlanker(bool)));
    connect(ui->pushAGC,SIGNAL(toggled(bool)),this,SLOT(slotAGC(bool)));
    connect(ui->pushVSC,SIGNAL(toggled(bool)),this,SLOT(slotVSC(bool)));


    // Connect filters
    connect(ui->push28k,SIGNAL(clicked()), this, SLOT(slotFilter28k()));
    connect(ui->push6k,SIGNAL(clicked()), this, SLOT(slotFilter6k()));
    connect(ui->push15k,SIGNAL(clicked()), this, SLOT(slotFilter15k()));
    connect(ui->push50k,SIGNAL(clicked()), this, SLOT(slotFilter50k()));
    connect(ui->push230k,SIGNAL(clicked()), this, SLOT(slotFilter230k()));

    // Connect Moudlation mode
    connect(ui->pushAM,SIGNAL(clicked()), this, SLOT(slotModulationAM()));
    connect(ui->pushFM,SIGNAL(clicked()), this, SLOT(slotModulationFM()));
    connect(ui->pushCW,SIGNAL(clicked()), this, SLOT(slotModulationCW()));
    connect(ui->pushWFM,SIGNAL(clicked()), this, SLOT(slotModulationWFM()));
    connect(ui->pushLSB,SIGNAL(clicked()), this, SLOT(slotModulationLSB()));
    connect(ui->pushUSB,SIGNAL(clicked()), this, SLOT(slotModulationUSB()));

    // Radio
    ui->buttonGroup->setId(ui->radio1,0);
    ui->buttonGroup->setId(ui->radio2,1);
    ui->signalRadio1->setOrientation(Qt::Horizontal,QwtThermo::BottomScale);

    // Frequency
    ui->layoutFrequency->addWidget(lcd);
    connect( lcd, SIGNAL(frequencyChanged(QString&)), this,SLOT(slotFrequency(QString&)));
#ifndef WIN32
    // Connect sound with demodulator
    sound->SetDemodulator(demodulator);
    //connect(sound,SIGNAL(dataBuffer(int16_t*,int)), demodulator, SLOT(slotDataBuffer(int16_t*,int)));
#endif
    // Connect spectrum widget
    connect(demodulator,SIGNAL(sigRawSamples(double*,double*,int)),mySpectrum,SLOT(slotRawSamples(double*,double*,int)));

    // Connect Demodulator to debug windows
    connect(demodulator,SIGNAL(sendData(QString)),this,SLOT(slotDemodulatorData(QString)));

    // Set threshold
    connect(ui->threshold, SIGNAL(valueChanged(int)), demodulator, SLOT(slotThreshold(int)));
    connect(ui->correlationLength,SIGNAL(sliderMoved(int)), demodulator, SLOT(slotSetCorrelationLength(int)));

    // Connect Decoder
    connect(ui->decoderList, SIGNAL(currentIndexChanged(int)), this, SLOT(slotDecoderChange(int)));

    // Channel change
    connect(ui->channel, SIGNAL(currentIndexChanged(int)), this, SLOT(slotChannelChange(int)));

    // Connect Scope type
    connect(ui->FFT, SIGNAL(clicked(bool)), this,SLOT(slotScopeChanged(bool)));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotRadioClicked(int)));
#ifndef WIN32
    connect(ui->pushSwitchSound,SIGNAL(clicked(bool)),this,SLOT(slotSwitchSound(bool)));
#endif

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
    connect(remote,SIGNAL(sigInitialize()), this, SLOT(powerOn()));
    connect(demodulator,SIGNAL(sigRawSamples(double*,double*,int)), remote, SLOT(controledRate(double*,double*,int)));
    mySpectrum->setAxis(0,16384,0,256);
#ifndef WIN32
    // Build menu settings
    // Add input list device
    QMenu *input = ui->menu_Settings->addMenu(tr("input"));
    QHashIterator<QString, int> in(sound->getDeviceList());
    QAction *action;
    while (in.hasNext()) {
        in.next();
        action = new QAction(in.key(), this);
        action->setCheckable(true);
        action->setObjectName(in.key());
        input->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(slotInputDevice()));
    }

    // add output list device
    QMenu *output = ui->menu_Settings->addMenu(tr("output"));
    QHashIterator<QString, int> out(sound->getDeviceList());
    while (out.hasNext()) {
        out.next();
        action = new QAction(out.key(), this);
        action->setCheckable(true);
        action->setObjectName(out.key());
        output->addAction(action);
        connect(action, SIGNAL(triggered()), this, SLOT(slotOutputDevice()));
    }
#endif
    // Connect load file
    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(slotLoadFile()));
    connect(ui->pushStopPlay, SIGNAL(clicked()), this, SLOT(slotStopPlay()));
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::powerOn()
{
    if (m_device->open())
    {
        qDebug() << "Connected";
    }

    if (cmd->getPower()) {
        cmd->setPower(false);
        status->setState(cmd->getPower());
        return;
    }
    cmd->setPower(false);
    sleep(1);
    cmd->setPower(true);
    sleep(2);
    // G105 ?
    dbgWin->slotSendSerial("G105");
    cmd->setRadio(0);
    cmd->setSquelch(255);
    cmd->setSoundVolume(0);
    cmd->setRadio(1);
    cmd->setSoundVolume(0);
    cmd->setSquelch(255);

    dbgWin->slotSendSerial("G2?");
    dbgWin->slotSendSerial("G4?");
    dbgWin->slotSendSerial("GE?");
    dbgWin->slotSendSerial("GD?");
    dbgWin->slotSendSerial("GA0?");
    dbgWin->slotSendSerial("GA1?");
    dbgWin->slotSendSerial("GA2?");
    dbgWin->slotSendSerial("GF?");
    sleep(1);
    dbgWin->slotSendSerial("G301");
/*  NOT NEEDED */
    dbgWin->slotSendSerial("J730000");
    dbgWin->slotSendSerial("J4600");
    dbgWin->slotSendSerial("J6600");

    // Init radio 0 Frequency;
    cmd->setRadio(0);
    slotModulationWFM();
    slotFilter230k();
    cmd->setFrequency(106500000);
    cmd->setSquelch(0);
    cmd->setVoiceControl(CCommand::eVSCOff);
    cmd->setIFShift(128);

    // Init radio 1 Frequency
    cmd->setRadio(1);
    cmd->setModulation(CCommand::eFM);
    cmd->setFilter(CCommand::e15k);
    cmd->setFrequency(145425000);
    cmd->setSquelch(0);
    cmd->setSoundVolume(0);
    cmd->setVoiceControl(CCommand::eVSCOff);
    cmd->setIFShift(128);

/*  NOT NEEDED */
    dbgWin->slotSendSerial("J4200");
    dbgWin->slotSendSerial("J4700");
    dbgWin->slotSendSerial("J6700");


    dbgWin->slotSendSerial("JC400");
    dbgWin->slotSendSerial("J7100");
    dbgWin->slotSendSerial("J720000");
    dbgWin->slotSendSerial("JC000");

/* */
    cmd->setRadio(0);
    cmd->setSoundMute(true);
    cmd->setSoundVolume(0);

    dbgWin->slotSendSerial("J8001");
    dbgWin->slotSendSerial("J8100");
    dbgWin->slotSendSerial("J8200");
    dbgWin->slotSendSerial("J8300");
    dbgWin->slotSendSerial("JC500");

    cmd->setRadio(0);
    cmd->setSquelch(255);
    cmd->setVoiceControl(CCommand::eVSCOff);
    cmd->setRadio(1);
    cmd->setSquelch(255);
    cmd->setVoiceControl(CCommand::eVSCOff);

    cmd->setRadio(0);
    cmd->setSoundVolume(0);
    cmd->setRadio(1);
    cmd->setSoundVolume(0);
    cmd->setSquelch(255);
    cmd->setRadioMode(CCommand::eBoth);
    //dbgWin->slotSendSerial("JB000");
    cmd->setRadio(1);
    cmd->setSquelch(255);
    cmd->setVoiceControl(CCommand::eVSCOff);

    cmd->setRadio(0);
    cmd->setSquelch(1);
    cmd->setVoiceControl(CCommand::eVSCOff);
    cmd->setRadio(1);
    cmd->setVoiceControl(CCommand::eVSCOff);
    cmd->setSquelch(1);

    cmd->setRadio(0);
    cmd->setSoundVolume(60);
    cmd->setSoundMute(false);
    //dbgWin->slotSendSerial("ME0000101081401050000");
    status->setState(cmd->getPower());

}

void CMainWindow::slotSendData(QString &data)
{
    m_device->write(data);
}

void CMainWindow::slotUpdateStatus()
{
    QString data("Data sent %1 bytes and received %2 bytes");
    status->slotUpdate(data.arg(m_device->log_t.dataSent).arg(m_device->log_t.dataReceive));
}

void CMainWindow::slotReceivedData(QString data)
{
    bool found = false;
    // Signal antenne 1
    if (data.contains("I1")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I1")+2,2).toUInt(&ok,16);
        if ((ok)  && (ui->radio1->isChecked())) {
            ui->signalRadio1->setValue(value);
            remote->sendData(QString("SA%1").arg(value));
        }
        found = true;
    }
    // Signal antenne 2
    if (data.contains("I5")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I5")+2,2).toUInt(&ok,16);
        if ((ok)  && (ui->radio2->isChecked())) {
            ui->signalRadio1->setValue(value);
            remote->sendData(QString("SB%1").arg(value));
        }
        found = true;
    }
    if (data.contains("H100")) {
        statusBar()->showMessage(tr("Offline"));
        status->setState(false);
        remote->sendData(QString("PWROFF"));
        found = true;
    }
    if (data.contains("H101")) {
        statusBar()->showMessage(tr("Online"));
        status->setState(true);
        remote->sendData(QString("PWRON"));
        found = true;
    }
    if (!found) {
        dbgWin->slotDebugSerial(data);
        remote->sendData(QString("DBG\r\n%1").arg(data));
    }


    // Update status bar
    QString info("Data sent %1 %3bytes and received %2 %4bytes");
    int received = m_device->log_t.dataReceive;
    int sent       = m_device->log_t.dataSent;
    QString receiveUnit("");
    QString sentUnit("");
    if (received > 9999)   { received = received / 1000.0; receiveUnit = "k"; }
    if (received > 999999) { received = received / 100000.0; receiveUnit = "M"; }

    if (sent > 9999)   { received = received / 1000.0; sentUnit = "k"; }
    if (sent > 999999) { received = received / 100000.0; sentUnit = "M";}


    status->slotUpdate(info.arg(sent).arg(received).arg(sentUnit).arg(receiveUnit));

}

void CMainWindow::slotVolume1(int value)
{
    cmd->setRadio(0);
    cmd->setSoundVolume(value);
    cmd->setRadio((ui->radio1->isChecked() == true ) ? 0 : 1);
}

void CMainWindow::slotVolume2(int value)
{
    cmd->setRadio(1);
    cmd->setSoundVolume(value);
    cmd->setRadio((ui->radio1->isChecked() == true ) ? 0 : 1);
}


void CMainWindow::slotSquelch(double value)
{
    cmd->setSquelch(value);
}

void CMainWindow::slotIF(double value)
{
    cmd->setIFShift(value);
}

void CMainWindow::slotFrequency(QString &value)
{
    if (value != "") {
        cmd->setFrequency(value.toInt());
    }
}

void CMainWindow::slotFilter28k()
{
    cmd->setFilter(CCommand::e28k);
    ui->push28k->setChecked(true);
}

void CMainWindow::slotFilter6k()
{
    cmd->setFilter(CCommand::e6k);
    ui->push6k->setChecked(true);
}

void CMainWindow::slotFilter15k()
{
    cmd->setFilter(CCommand::e15k);
    ui->push15k->setChecked(true);
}

void CMainWindow::slotFilter50k()
{
    cmd->setFilter(CCommand::e50k);
    ui->push50k->setChecked(true);
}

void CMainWindow::slotFilter230k()
{
    cmd->setFilter(CCommand::e230k);
    ui->push230k->setChecked(true);

}

void CMainWindow::slotModulationAM()
{
    cmd->setModulation(CCommand::eAM);
    ui->pushAM->setChecked(true);
}

void CMainWindow::slotModulationFM()
{
    cmd->setModulation(CCommand::eFM);
    ui->pushFM->setChecked(true);
}

void CMainWindow::slotModulationWFM()
{
    cmd->setModulation(CCommand::eWFM);
    ui->pushWFM->setChecked(true);

}

void CMainWindow::slotModulationLSB()
{
    cmd->setModulation(CCommand::eLSB);
    ui->pushLSB->setChecked(true);

}

void CMainWindow::slotModulationUSB()
{
    cmd->setModulation(CCommand::eUSB);
    ui->pushUSB->setChecked(true);
}

void CMainWindow::slotModulationCW()
{
    cmd->setModulation(CCommand::eCW);
    ui->pushCW->setChecked(true);
}

void CMainWindow::slotRadioClicked(int value)
{
    cmd->setRadio(value);

    // Init button according to radio settings
    QString freq("%1");
    freq = freq.arg(cmd->getFrequency(),10);
    qDebug() << "freq  " << freq;
    lcd->setFrequency(freq);
    ui->knobIF->setValue(cmd->getIFShift());
    ui->knobSquelch->setValue(cmd->getSquelch());
    switch(cmd->getModulation()) {
        case CCommand::eAM : ui->pushAM->setChecked(true);   break;
        case CCommand::eFM : ui->pushFM->setChecked(true);   break;
        case CCommand::eWFM : ui->pushWFM->setChecked(true); break;
        case CCommand::eCW  : ui->pushCW->setChecked(true);  break;
        case CCommand::eLSB : ui->pushLSB->setChecked(true); break;
        case CCommand::eUSB : ui->pushUSB->setChecked(true); break;
    }
    switch(cmd->getFilter()) {
        case CCommand::e28k : ui->push28k->setChecked(true);   break;
        case CCommand::e6k  : ui->push6k->setChecked(true);    break;
        case CCommand::e15k : ui->push15k->setChecked(true);   break;
        case CCommand::e50k : ui->push50k->setChecked(true);   break;
        case CCommand::e230k : ui->push230k->setChecked(true); break;
    }
}

void CMainWindow::slotSwitchSound(bool value)
{
    demodulator->initBuffer(32768);
#ifdef WITH_SNDFILE || WITH_PORTAUDIO || WITH_PULSEAUDIO
    if (value == true) {
        sound->start();
    }
    else {
        sound->setRunning(false);
        sound->terminate();
    }
#endif
}

void CMainWindow::slotNoiseBlanker(bool value)
{
    cmd->setNoiseBlanker(value);
}

void CMainWindow::slotAGC(bool value)
{
   cmd->setAutomaticGainControl(value);

}

void CMainWindow::slotVSC(bool value)
{
    cmd->setVoiceControl(value);
}

void CMainWindow::slotDemodulatorData(QString data)
{
    ui->decoderText->setPlainText(ui->decoderText->toPlainText()+data);
    //ui->decoderText->append(data);
    QScrollBar *sb = ui->decoderText->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void CMainWindow::slotDecoderChange(int value)
{
    int channel = ui->channel->currentIndex();
    qDebug() << "CMainWindow::slotDecoderChange(" << value << ")";
    demodulator->slotSetDemodulator(value,channel,16384);
    // Connect Demodulator to debug windows
    connect(demodulator->getDemodulatorFromChannel(channel),SIGNAL(dumpData(double*,double*,int)),myDecoder,SLOT(slotRawSamples(double*,double*,int)));
    connect(mySpectrum, SIGNAL(frequency(double)), demodulator->getDemodulatorFromChannel(channel), SLOT(slotFrequency(double)));
    myDecoder->setAxis(0,256,0.0,30.0);
}

void CMainWindow::slotChannelChange(int value)
{
    qDebug() << "CMainWindow::slotChannelChange(" << value << ")";
    //demodulator->slotSetDemodulator(ui->decoderList->currentIndex(), value, 16384);
}

void CMainWindow::slotScopeChanged(bool value)
{
    if (value) {
        demodulator->setScopeType(1);
        mySpectrum->setAxis(0,256,0,50);
    }
    else {
        demodulator->setScopeType(0);
        mySpectrum->setAxis(0,1024,0,256);
    }
}

void CMainWindow::slotRemoteData(QString &data)
{

}

void CMainWindow::slotInputDevice()
{
#ifndef WIN32
    sound->selectInputDevice(QObject::sender()->objectName());
#endif
}

void CMainWindow::slotOutputDevice()
{
#ifndef WIN32
    sound->selectOutputDevice(QObject::sender()->objectName());
#endif
}

void CMainWindow::slotLoadFile()
{
    // File dialog chooser
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Open Sound"), QDir::homePath(), tr("Sound Files (*.wav *.flac *.au *.voc *.ogg)"));
    // Close sound card reader
#ifndef WIN32
    sound->terminate();
#endif
#ifdef WITH_SNDFILE
    if (!fileName.isEmpty()) {
        // Create new sound reader from file
        sound = new CSoundFile(this);
        sound->Load(fileName);
        sound->SetDemodulator(demodulator);
        sound->setRunning(true);
    }
#endif
    slotSwitchSound(true);
}

void CMainWindow::slotStopPlay()
{
    sound->setRunning(false);
    sleep(5);
    sound->terminate();
    delete sound;
#ifdef WITH_PULSEAUDIO
    sound  = new CPulseSound(this);
#endif
#ifdef WITH_PORTAUDIO
    sound  = new CPortAudio(this);
#endif
    sound->SetDemodulator(demodulator);
}
