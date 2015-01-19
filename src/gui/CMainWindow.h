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
#include "CBackend.h"
#include "CDebugWindow.h"
#include "CLcdWidget.h"
#include "CStatusWidget.h"
#include "CSpectrumWidget.h"
#include "CBandScope.h"
#include "CDisplay.h"
#include "CFft.h"
#include "CDlgSettings.h"

#define MAX_RADIO 2

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
        void slotReceivedData(CCommand::status_t status);
        void slotVolume1(double value);
        void slotSquelch(double value);
        void slotIF(double value);
        void slotFrequency(QString value);
        void slotUpdateStatus();
        // Filter switch
        void slotFilter(int filter);
        // Modulation switch
        void slotModulation(int mode);
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
        // Step
        void slotStepUp();
        void slotStepDown();
        // Display Settings
        void slotSettings();
        // Slot Threshold
        void slotThreshold(double threshold);

    private:
        Ui::MainWindow *ui;

        // Bandscope
        int bandscopeActivate;

        // Backend job
        CBackend *backend;

        // Windows
        CDebugWindow *dbgWin;

        //Widgets
        CLcdWidget *lcd1;
        CLcdWidget *lcd2;

        CStatusWidget *statusbar;

        // Timer
        QTimer *statusTimer;

        // spectrum
        CSpectrumWidget *mySpectrum;

        // dump decoder view
        CSpectrumWidget *myDecoder;

        // Bandscope
        CBandScope *myBandScope;

        //QToolbar
        QToolBar *dock;

        // Display
        CDisplay *display;

        // Restore settings
        void restoreSettings();

        // Save settings
        void saveSettings();

        // Restore sound
        void restoreSound();

        // Save sound
        void saveSound();

        // Connect signals
        void connectSignals();

        // List of radio
        QList<CCommand::radio_t*> radioList;

        // Decoder settings struct
        CDecoder::decoder_t decoder;

        // Bandscope settings
        CCommand::bandscope_t bandscope;

        // Status of device
        CCommand::status_t status;

};

extern CMainWindow *theMainWindow;

#endif                           //CMAINWINDOW_H
