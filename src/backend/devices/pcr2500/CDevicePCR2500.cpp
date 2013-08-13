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
#include "CDevicePCR2500.h"

#include <QtGui>
#ifdef WIN32
#define sleep Sleep
#endif

CDevicePCR2500::CDevicePCR2500(const QString& serialport,
                         const QString& baudrate,
                         QObject * parent)
: IDevice("PCR2500",parent)
, serialport(serialport)
, haveSeenData(false)
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

    //tty.setTimeout(2, 500);
    tty.setTimeout(0, 10);
    tty.enableSending();
    tty.enableReceiving();
    tty.setPort(serialport);

    watchdog = new QTimer(this);
    connect(watchdog, SIGNAL(timeout()), this, SLOT(slotWatchdog()));

    connect(&tty, SIGNAL(newDataReceived(const QByteArray &)), this, SLOT(slotNewDataReceived(const QByteArray &)));

    log_t.isConnected = false;
    log_t.dataCount = 0;
    log_t.dataReceive = 0;
    log_t.dataSent = 0;

}


CDevicePCR2500::~CDevicePCR2500()
{
    tty.close();
}

void CDevicePCR2500::slotNewDataReceived(const QByteArray &dataReceived)
{
    log_t.dataReceive += dataReceived.size();
    //qDebug() << "Data received " << dataReceived.data() << " length " << dataReceived.length();
    emit sigData(QString(dataReceived));
    haveSeenData = true;
}

void CDevicePCR2500::slotWatchdog()
{
    qDebug() << "Watchdog";
    if(tty.isOpen() && haveSeenData)
    {
        haveSeenData = false;
        return;
    }

}

void CDevicePCR2500::write(QString &data)
{
    QByteArray byteArray(data.toLocal8Bit());
    log_t.dataSent += byteArray.size();
    byteArray.append("\n");
    tty.sendData(byteArray);
}


bool CDevicePCR2500::open()
{
    if (tty.open()) {
        // Wait a little  before ICOM initialize serial
        qDebug() << "opened";
        // Wait for data
        sleep(5);
        if (tty.receiveData() == 1 )
        {
            qDebug() << "receiveData() activate";
            haveSeenData = false;
            log_t.isConnected = true;
            watchdog->start(10000);
            return true;
        }
        return false;
    } else
        return false;

}

void CDevicePCR2500::close()
{
    tty.close();
}

QString CDevicePCR2500::WriteAndRead(QString cmd, int timeout)
{
    return QString("");    //while(tty.receiveData()>
}

void CDevicePCR2500::setBaudRate(const QString& baudrate)
{
    enum BaudRateType eBaudrate = BAUD4800;
    if (baudrate.compare("9600")   == 0) { eBaudrate = BAUD9600;   }
    if (baudrate.compare("19200")  == 0) { eBaudrate = BAUD19200;  }
    if (baudrate.compare("38400")  == 0) { eBaudrate = BAUD38400;  }
    if (baudrate.compare("57600")  == 0) { eBaudrate = BAUD57600;  }
    if (baudrate.compare("115200") == 0) { eBaudrate = BAUD115200; }
    tty.setBaudRate(eBaudrate);   //BaudRate

}
