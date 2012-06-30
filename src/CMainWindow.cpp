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

    statusBar()->addPermanentWidget(status);

    m_device = new CDevicePCR2500("/dev/ttyUSB0", "38400", this);

    connect(ui->pushPower, SIGNAL(clicked()), this, SLOT(powerOn()));
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(cmd,SIGNAL(sendData(QString&)),this,SLOT(slotSendData(QString&)));
    connect(ui->volume, SIGNAL(valueChanged(int)), this,SLOT(slotVolume(int)));
    connect(ui->knobSquelch,SIGNAL(valueChanged(double)), this, SLOT(slotSquelch(double)));
    connect(ui->pushEnter,SIGNAL(clicked()), this,SLOT(slotFrequency()));

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
    sleep(1);
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
    cmd->setModulation(CCommand::eWFM);
    cmd->setFilter(CCommand::e230k);
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
    if (data.contains("I1")) {
        double value;
        bool ok;
        value = data.mid(data.indexOf("I1")+2,2).toUInt(&ok,16);
        if (ok) {
            ui->signalRadio1->setValue(value);
            found = true;
        }
    }
    if (data.contains("H100")) {
        statusBar()->showMessage(tr("Offline"));
        //status->setState(false);
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
    QString info("Data sent %1 bytes and received %2 bytes");
    status->slotUpdate(info.arg(m_device->log_t.dataSent).arg(m_device->log_t.dataReceive));

}

void CMainWindow::slotVolume(int value)
{
    cmd->setSoundVolume(value);
}


void CMainWindow::slotSquelch(double value)
{
    cmd->setSquelch(value);
}

void CMainWindow::slotFrequency()
{
    bool ok;
    if (ui->frequencyEnter->text() != "") {
        cmd->setRadio(0);
        cmd->setFilter(CCommand::e230k);
        cmd->setModulation(CCommand::eWFM);
        cmd->setFrequency(ui->frequencyEnter->text().toInt(&ok,10));
        ui->frequency->display(ui->frequencyEnter->text().toInt(&ok,10));
        //ui->frequency->intValue();
    }
}
