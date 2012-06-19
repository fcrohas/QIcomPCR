/**********************************************************************************************
Copyright (C) 2009 Oliver Eichler oliver.eichler@gmx.de

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
#include "CDevicePCR2500.h"

#include <QtGui>

CDevicePCR2500::CDevicePCR2500(const QString& serialport,
                         const QString& baudrate,
                         QObject * parent)
: IDevice("PCR2500",parent)
, serialport(serialport)
{
    enum BaudRateType eBaudrate = BAUD4800;
    if (baudrate.compare("9600")   == 0) { eBaudrate = BAUD9600;   }
    if (baudrate.compare("19200")  == 0) { eBaudrate = BAUD19200;  }
    if (baudrate.compare("38400")  == 0) { eBaudrate = BAUD38400;  }
    if (baudrate.compare("57600")  == 0) { eBaudrate = BAUD57600;  }
    if (baudrate.compare("115200") == 0) { eBaudrate = BAUD115200; }

    tty.setBaudRate(eBaudrate);   //BaudRate
    tty.setDataBits(DATA_8);     //DataBits
    tty.setParity(PAR_NONE);     //Parity
    tty.setStopBits(STOP_1);     //StopBits
    tty.setFlowControl(FLOW_OFF);//FlowControl

    tty.setTimeout(0, 10);
    tty.enableReceiving();
    tty.setPort(serialport);

    watchdog = new QTimer(this);
    connect(watchdog, SIGNAL(timeout()), this, SLOT(slotWatchdog()));

    connect(&tty, SIGNAL(newDataReceived(const QByteArray &)), this, SLOT(slotNewDataReceived(const QByteArray &)));
}


CDevicePCR2500::~CDevicePCR2500()
{
    tty.close();
}

void CDevicePCR2500::slotNewDataReceived(const QByteArray &dataReceived)
{
    int i;

    //log.count_bytes += dataReceived.size();

    for(i = 0; i < dataReceived.size(); ++i)
    {

        if(dataReceived[i] == '\n')
        {
            line = line.trimmed();
            /*
            if (isChecksumValid())
            {
                //log.count_nmea++;
                //decode();
            }
            else
            {
                line.clear();
            }
            */
        }
        else
        {
            line += dataReceived[i];
        }
    }

    //emit sigLiveLog(log);
}

void CDevicePCR2500::slotWatchdog()
{
    /*
    if(tty.isOpen() && haveSeenData)
    {
        haveSeenData = false;
        return;
    }

    setLiveLog(false);
    */
}
