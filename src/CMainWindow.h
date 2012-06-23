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
#include "CDevicePCR2500.h"
#include "CDebugWindow.h"

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
        void sendData(QString &data);

    private:
        Ui::MainWindow *ui;
        CDevicePCR2500 * m_device;
        CDebugWindow *dbgWin;

};

extern CMainWindow * theMainWindow;

#endif                           //CMAINWINDOW_H
