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
#ifndef ISOUND_H
#define ISOUND_H

#include <QObject>
#include <QThread>
#include <sndfile.h>
#include "CDemodulator.h"

class ISound : public QThread
{
    Q_OBJECT
public:
    explicit ISound(QObject *parent = 0);
    ~ISound();

    // Demodulator
    void SetDemodulator(CDemodulator *value);
    CDemodulator *GetDemodulator();

    virtual void DecodeBuffer(int16_t *buffer, int size);
    virtual void setRunning(bool value);
    virtual QHash<QString,int> getDeviceList();
    virtual void selectInputDevice(QString device);
    virtual void selectOutputDevice(QString device);
    virtual bool Load(QString &file);
    virtual void Record(QString &filename, bool start);

signals:
    
public slots:
    virtual void setChannel(uint value);

private:
    CDemodulator *demod;
    SNDFILE*	pFile;
    QString data;
};

#endif // ISOUND_H
