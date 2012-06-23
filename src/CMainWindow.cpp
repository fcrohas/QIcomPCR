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

    dbgWin = new CDebugWindow(this,ui);

    m_device = new CDevicePCR2500("/dev/ttyUSB0", "38400", this);

    connect(ui->pushPower, SIGNAL(clicked()), this, SLOT(powerOn()));
    connect(m_device, SIGNAL(sigData(QString)), dbgWin, SLOT(slotDebugSerial(QString)));
    connect(dbgWin,SIGNAL(sendData(QString&)),this,SLOT(sendData(QString&)));

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
    QString data;
    data = "H101";
    m_device->write(data);
    data = "H1?";
    m_device->write(data);
    data = "G105";
    m_device->write(data);
    data = "J4000";
    m_device->write(data);
    data = "J6000";
    m_device->write(data);
    data = "J61FF";
    m_device->write(data);
    data = "G2?";
    m_device->write(data);
    data = "G4?";
    m_device->write(data);
    data ="GE?";
    m_device->write(data);
    data ="GD?";
    m_device->write(data);
    data = "GA0?";
    m_device->write(data);
    data = "GA1?";
    m_device->write(data);
    data = "GA2?";
    m_device->write(data);
    data = "GF?";
    m_device->write(data);
    data = "G301";
    m_device->write(data);
}

void CMainWindow::sendData(QString &data)
{
    m_device->write(data);
}
