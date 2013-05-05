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
#ifndef IDEVICE_H
#define IDEVICE_H

#include <QObject>
#include <QList>
#include <QPointer>

class QProgressDialog;

class IDevice : public QObject
{
    Q_OBJECT;
    public:
        IDevice(const QString& devkey, QObject * parent);
        virtual ~IDevice();
        void write(QString &data);


    signals:
        void sigData(QString data);

    protected:
        void createProgress(const QString& title, const QString& text, int max);

        QPointer<QProgressDialog> progress;

};
#endif                           //IDEVICE_H
