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

#ifndef CMAINWINDOW_H
#define CMAINWINDOW_H

#include <QMainWindow>
#include <QAudioDeviceInfo>
#include <QAudioInput>
#include <QAudioOutput>
#include "CCommand.h"
#include "CDevicePCR2500.h"
#include "CDebugWindow.h"
#include "CLcdWidget.h"
#include "CStatusWidget.h"

namespace Ui {
    class MainWindow;
}

class IDevice;

class CMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit CMainWindow(QWidget *parent = 0);
        ~CMainWindow();

    protected:

    private slots:
        void powerOn();
        void slotSendData(QString &data);
        void slotReceivedData(QString data);
        void slotVolume(int value);
        void slotSquelch(double value);
        void slotFrequency(QString&value);
        void slotUpdateStatus();
        // Filter switch
        void slotFilter28k();
        void slotFilter6k();
        void slotFilter15k();
        void slotFilter50k();
        void slotFilter230k();
        // Modulation switch
        void slotModulationAM();
        void slotModulationFM();
        void slotModulationWFM();
        void slotModulationCW();
        void slotModulationLSB();
        void slotModulationUSB();
        // Radio
        void slotRadioClicked(int);
        void slotNoiseBlanker();
        void slotAGC();
        void slotVSC();
        // Sound Control
        void slotSwitchSound();


    private:
        Ui::MainWindow *ui;

        // CommandBuilder
        CCommand * cmd;

        // Device driver
        CDevicePCR2500 * m_device;

        // Windows
        CDebugWindow *dbgWin;

        //Widgets
        CLcdWidget *lcd;
        CStatusWidget *status;

        // Timer
        QTimer *statusTimer;

        // Audio management
        QAudioOutput *soundOutput;
        QAudioInput  *soundInput;

};

extern CMainWindow * theMainWindow;

#endif                           //CMAINWINDOW_H
