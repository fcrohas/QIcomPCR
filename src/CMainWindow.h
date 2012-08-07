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
#ifndef WIN32
#include "CPulseSound.h"
#endif
#include "CStatusWidget.h"

namespace Ui {
    class MainWindow;
}

class IDevice;

class Plotter : public QWidget
{
    Q_OBJECT
public:
    explicit Plotter(QWidget *parent =0);
    void setRawSamples(double *xval, double *yval,int size);
private:
    QwtPlotCurve *spectro;
    QHBoxLayout *hboxLayout;
    QwtPlot *qwtPlot;

    void setupUi(QWidget *widget)
    {
                QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
                sizePolicy.setHorizontalStretch(20);
                sizePolicy.setVerticalStretch(20);
                sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
                widget->setSizePolicy(sizePolicy);
                widget->setAutoFillBackground(false);
                hboxLayout = new QHBoxLayout(widget);
                qwtPlot = new QwtPlot(widget);
                qwtPlot->setSizePolicy(sizePolicy);
                hboxLayout->addWidget(qwtPlot);

                //QMetaObject::connectSlotsByName(widget);
    }
};

class CMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit CMainWindow(QWidget *parent = 0);
        ~CMainWindow();

    protected:

    public slots:
        void slotDataBuffer(double *xval, double *yval);

    private slots:
        void powerOn();
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
#ifndef WIN32
        CPulseSound *sound;
#endif
        CStatusWidget *status;

        // Timer
        QTimer *statusTimer;

        // Audio management
        QAudioOutput *soundOutput;
        QAudioInput  *soundInput;

        // spectrum
        Plotter *myPlot;

};

extern CMainWindow * theMainWindow;

#endif                           //CMAINWINDOW_H
