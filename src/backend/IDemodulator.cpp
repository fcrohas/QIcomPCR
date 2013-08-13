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
#include "IDemodulator.h"

IDemodulator::IDemodulator(QObject *parent) :
    QObject(parent)
{
}

uint IDemodulator::getChannel()
{
    return 0;
}

uint IDemodulator::getDataSize()
{
    return 0;
}

void IDemodulator::decode(int16_t *buffer, int size, int offset)
{

}

void IDemodulator::decode(uchar *buffer, int size, int offset)
{

}

uint IDemodulator::getBufferSize()
{
    return 0;
}

void IDemodulator::slotFrequency(double value)
{
    return;
}

void IDemodulator::slotBandwidth(double value)
{
    return;
}

void IDemodulator::setThreshold(double value)
{

}

void IDemodulator::setCorrelationLength(int value)
{

}
