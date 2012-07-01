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
#include "CDevicePCR2500.h"
#include "version.h"

#include <QtGui>
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32
#include <io.h>
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

    status = new CStatusWidget(this);
    lcd    = new CLcdWidget(this);

    statusBar()->addPermanentWidget(status);

    m_device = new CDevicePCR2500("/dev/ttyUSB0", "38400", this);

    connect(ui->pushPower, SIGNAL(clicked()), this, SLOT(powerOn()));
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(cmd,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(ui->volume, SIGNAL(valueChanged(int)), this,SLOT(slotVolume(int)));
    connect(ui->knobSquelch,SIGNAL(valueChanged(double)), this, SLOT(slotSquelch(double)));
    //connect(ui->pushEnter,SIGNAL(clicked()), this,SLOT(slotFrequency()));

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

    // Frequency
    ui->layoutFrequency->addWidget(lcd);
    connect( lcd, SIGNAL(frequencyChanged(QString&)), this,SLOT(slotFrequency(QString&)));

    connect(ui->buttonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotRadioClicked(int)));

    if (m_device->open())
    {
        qDebug() << "Connected";
    }
}

CMainWindow::~CMainWindow()
{
}

void CMainWindow::powerOn()
{
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
/*  NOT NEEDED
    dbgWin->slotSendSerial("J730000");
    dbgWin->slotSendSerial("J4600");
    dbgWin->slotSendSerial("J6600");
*/
    // Init radio 0 Frequency;
    cmd->setRadio(0);
    slotModulationWFM();
    slotFilter230k();
    cmd->setFrequency(106500000);
    cmd->setSquelch(1);
    cmd->setVoiceControl(CCommand::eVSCOff);

    // Init radio 1 Frequency
    cmd->setRadio(1);
    cmd->setModulation(CCommand::eFM);
    cmd->setFilter(CCommand::e50k);
    cmd->setFrequency(440000000);
    cmd->setSquelch(255);
    cmd->setVoiceControl(CCommand::eVSCOff);

    cmd->setRadio(0);
/*  NOT NEEDED
    dbgWin->slotSendSerial("J4200");
    dbgWin->slotSendSerial("J4700");
    dbgWin->slotSendSerial("J6700");


    dbgWin->slotSendSerial("JC400");
    dbgWin->slotSendSerial("J7100");
    dbgWin->slotSendSerial("J720000");
    dbgWin->slotSendSerial("JC000");
    */
/*
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
*/
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
        if (ok) {
            ui->signalRadio1->setValue(value);
            found = true;
        }
    }
    // Signal antenne 2
    if (data.contains("I5")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I5")+2,2).toUInt(&ok,16);
        if (ok) {
            ui->signalRadio1->setValue(value);
            found = true;
        }
    }
    if (data.contains("H100")) {
        statusBar()->showMessage(tr("Offline"));
        status->setState(false);
        found = true;
    }
    if (data.contains("H101")) {
        statusBar()->showMessage(tr("Online"));
        status->setState(true);
        found = true;
    }
    if (!found) {
        dbgWin->slotDebugSerial(data);
    }

    // Update status bar
    QString info("Data sent %1 %3bytes and received %2 %4bytes");
    int received = m_device->log_t.dataReceive;
    int sent       = m_device->log_t.dataSent;
    QString receiveUnit("");
    QString sentUnit("");
    if (received > 9999)   { received = received / 1000; receiveUnit = "k"; }
    if (received > 999999) { received = received / 100000; receiveUnit = "M"; }

    if (sent > 9999)   { received = received / 1000; sentUnit = "k"; }
    if (sent > 999999) { received = received / 100000; sentUnit = "M";}


    status->slotUpdate(info.arg(sent).arg(received).arg(sentUnit).arg(receiveUnit));

}

void CMainWindow::slotVolume(int value)
{
    cmd->setSoundVolume(value);
}


void CMainWindow::slotSquelch(double value)
{
    cmd->setSquelch(value);
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
    ui->push6k->setChecked(false);
    ui->push15k->setChecked(false);
    ui->push50k->setChecked(false);
    ui->push230k->setChecked(false);
}

void CMainWindow::slotFilter6k()
{
    cmd->setFilter(CCommand::e6k);
    ui->push28k->setChecked(false);
    ui->push6k->setChecked(true);
    ui->push15k->setChecked(false);
    ui->push50k->setChecked(false);
    ui->push230k->setChecked(false);
}

void CMainWindow::slotFilter15k()
{
    cmd->setFilter(CCommand::e15k);
    ui->push28k->setChecked(false);
    ui->push6k->setChecked(false);
    ui->push15k->setChecked(true);
    ui->push50k->setChecked(false);
    ui->push230k->setChecked(false);
}

void CMainWindow::slotFilter50k()
{
    cmd->setFilter(CCommand::e50k);
    ui->push28k->setChecked(false);
    ui->push6k->setChecked(false);
    ui->push15k->setChecked(false);
    ui->push50k->setChecked(true);
    ui->push230k->setChecked(false);

}

void CMainWindow::slotFilter230k()
{
    cmd->setFilter(CCommand::e230k);
    ui->push28k->setChecked(false);
    ui->push6k->setChecked(false);
    ui->push15k->setChecked(false);
    ui->push50k->setChecked(false);
    ui->push230k->setChecked(true);

}


void CMainWindow::slotModulationAM()
{
    cmd->setModulation(CCommand::eAM);
    ui->pushAM->setChecked(true);
    ui->pushFM->setChecked(false);
    ui->pushWFM->setChecked(false);
    ui->pushCW->setChecked(false);
    ui->pushLSB->setChecked(false);
    ui->pushUSB->setChecked(false);
}

void CMainWindow::slotModulationFM()
{
    cmd->setModulation(CCommand::eFM);
    ui->pushAM->setChecked(false);
    ui->pushFM->setChecked(true);
    ui->pushWFM->setChecked(false);
    ui->pushCW->setChecked(false);
    ui->pushLSB->setChecked(false);
    ui->pushUSB->setChecked(false);
}

void CMainWindow::slotModulationWFM()
{
    cmd->setModulation(CCommand::eWFM);
    ui->pushAM->setChecked(false);
    ui->pushFM->setChecked(false);
    ui->pushWFM->setChecked(true);
    ui->pushCW->setChecked(false);
    ui->pushLSB->setChecked(false);
    ui->pushUSB->setChecked(false);

}

void CMainWindow::slotModulationLSB()
{
    cmd->setModulation(CCommand::eLSB);
    ui->pushAM->setChecked(false);
    ui->pushFM->setChecked(false);
    ui->pushWFM->setChecked(false);
    ui->pushCW->setChecked(false);
    ui->pushLSB->setChecked(true);
    ui->pushUSB->setChecked(false);

}

void CMainWindow::slotModulationUSB()
{
    cmd->setModulation(CCommand::eUSB);
    ui->pushAM->setChecked(false);
    ui->pushFM->setChecked(false);
    ui->pushWFM->setChecked(false);
    ui->pushCW->setChecked(false);
    ui->pushLSB->setChecked(false);
    ui->pushUSB->setChecked(true);
}

void CMainWindow::slotModulationCW()
{
    cmd->setModulation(CCommand::eCW);
    ui->pushAM->setChecked(false);
    ui->pushFM->setChecked(false);
    ui->pushWFM->setChecked(false);
    ui->pushCW->setChecked(true);
    ui->pushLSB->setChecked(false);
    ui->pushUSB->setChecked(false);
}

void CMainWindow::slotRadioClicked(int value)
{
    qDebug() << "clicked " << value;
    cmd->setRadio(value);
}
