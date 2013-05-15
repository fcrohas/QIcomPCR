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
#include "CRemoteControl.h"
#include "CLcdWidget.h"
#ifdef WITH_PULSEAUDIO
#include "CPulseSound.h"
#endif
#ifdef WITH_PORTAUDIO
#include "CPortAudio.h"
#endif
#ifdef WITH_SNDFILE
#include "CSoundFile.h"
#endif
#include "CStatusWidget.h"
#include "CSpectrumWidget.h"
#include "CBandScope.h"
#include "CDemodulator.h"
#include "CFft.h"
#include "ISound.h"

// Struct of value for bandscope
const int bandwidth[7] = {5000000,2000000,1000000,500000,200000,100000,50000};
const int stepsize[10] = {100000,50000,25000,15000,12500,10000,6000,5000,2500,1000};


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

    public slots:

    private slots:
        void powerOn(bool value);
        void slotSendData(QString &data);
        void slotReceivedData(QString data);
        void slotVolume1(int value);
        void slotVolume2(int value);
        void slotSquelch(double value);
        void slotIF(double value);
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
        void slotNoiseBlanker(bool value);
        void slotAGC(bool value);
        void slotVSC(bool value);
        // Sound Control
        void slotSwitchSound(bool value);
        // Demodulator text output
        void slotDemodulatorData(QString data);
        // Decoder change event
        void slotDecoderChange(int value);
        // Sound channel
        void slotChannelChange(int value);
        // Scope
        void slotScopeChanged(int value);
        // Remote
        void slotRemoteData(QString &data);
        // Input Device selection
        void slotInputDevice();
        // Output Device selection
        void slotOutputDevice();
        // Load sound file
        void slotLoadFile();
        // Stop sound playing
        void slotStopPlay(bool value);
        // Record audio
        void slotRecordAudio(bool value);
        // Window FFT function
        void slotWindowFunction(QString value);
        // Refresh Rate
        void slotRefreshRate(int value);
        // BandScope
        void slotBandScope(bool value);
        // Bandscope width
        void slotBandScopeWidth(int value);
        // Bandscope step
        void slotBandScopeStep(int value);

    private:
        Ui::MainWindow *ui;

        // CommandBuilder
        CCommand * cmd;

        // Windows
        CDebugWindow *dbgWin;

        //Widgets
        CLcdWidget *lcd;
#if defined(WITH_SNDFILE) || defined(WITH_PORTAUDIO) || defined(WITH_PULSEAUDIO)
        ISound *sound;
#endif
        CStatusWidget *status;

        // Timer
        QTimer *statusTimer;

        // Audio management
        QAudioOutput *soundOutput;
        QAudioInput  *soundInput;

        // spectrum
        CSpectrumWidget *mySpectrum;

        // dump decoder view
        CSpectrumWidget *myDecoder;

        // Bandscope
        CBandScope *myBandScope;

        // Demodulator
        CDemodulator *demodulator;

        // Remote Control
        CRemoteControl *remote;

        //QToolbar
        QToolBar *dock;

};

extern CMainWindow *theMainWindow;

#endif                           //CMAINWINDOW_H