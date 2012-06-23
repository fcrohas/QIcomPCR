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

#ifndef CDEVICEPCR2500_H
#define CDEVICEPCR2500_H

#include "IDevice.h"
#include <ManageSerialPort.h>
#include <QTimer>

class CDevicePCR2500 : public IDevice
{
    Q_OBJECT;
    public:
        CDevicePCR2500(const QString& serialport, const QString& baudrate, QObject * parent);
        virtual ~CDevicePCR2500();
        void write(QString &data);
        bool open();

    private slots:
        void slotNewDataReceived(const QByteArray &dataReceived);
        void slotWatchdog();

    private:
        QString serialport;
        QString line;
        ManageSerialPort tty;

        QTimer * watchdog;

        bool haveSeenData;

};
#endif                           //CDEVICENMEA_H
