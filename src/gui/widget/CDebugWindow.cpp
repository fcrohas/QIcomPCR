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
#include "CDebugWindow.h"
#include <QScrollBar>

CDebugWindow::CDebugWindow(QObject *parent, Ui::MainWindow *gui) :
    QObject(parent)
{
    ui = gui;
    connect(ui->pushSend, SIGNAL(clicked()), this, SLOT(slotSendSerial()));
    ui->serialOutput->setReadOnly(true);
}

void CDebugWindow::writeConsole(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    QScrollBar *sb = ui->serialOutput->verticalScrollBar();
    sb->setValue(sb->maximum());

}

void CDebugWindow::slotDebugSerial(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    QScrollBar *sb = ui->serialOutput->verticalScrollBar();
    sb->setValue(sb->maximum());
}


void CDebugWindow::slotSendSerial()
{
    QString data(ui->serialInput->text());
    ui->serialOutput->insertPlainText(data+"\n");
    ui->serialInput->clear();
    emit sendData(data);
}

void CDebugWindow::slotSendSerial(QString data)
{
    ui->serialOutput->insertPlainText(data+"\n");
    ui->serialInput->clear();
    emit sendData(data);
}
