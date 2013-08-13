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
#ifndef CDEBUGWINDOW_H
#define CDEBUGWINDOW_H

#include <QObject>
#include "ui_CMainWindow.h"

namespace Ui {
    class MainWindow;
}

class CDebugWindow : public QObject
{
    Q_OBJECT
public:
    explicit CDebugWindow(QObject *parent = 0, Ui::MainWindow *gui=0);

    void writeConsole(QString data);
signals:
    void sendData(QString &data);
public slots:
    void slotDebugSerial(QString data);
    // send
    void slotSendSerial();
    void slotSendSerial(QString data);
private:
    Ui::MainWindow *ui;
};

#endif // CDEBUGWINDOW_H
