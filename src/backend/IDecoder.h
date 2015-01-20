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
#ifndef IDECODER_H
#define IDECODER_H

#include <QObject>
#include <stdint.h>

class IDecoder : public QObject
{
    Q_OBJECT
public:
    explicit IDecoder(QObject *parent = 0);
    virtual void decode(uchar *buffer, int size, int offset);
    virtual void decode(int16_t *buffer, int size, int offset);
    virtual uint getDataSize();
    virtual uint getChannel();
    virtual uint getBufferSize();
    virtual void setCorrelationLength(int value);


signals:
    void sendData(QString data);
    void dumpData(double*,double*,int);
    
public slots:
    void slotFrequency(double value);
    void slotBandwidth(double value);
    void setThreshold(double value);
};

#endif // IDEMODULATOR_H
