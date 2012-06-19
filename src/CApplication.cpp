/**********************************************************************************************
  Copyright (C) 2011 Michael Klein <michael.klein@puffin.lb.shuttle.de>

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

#include "CApplication.h"
#include "CMainWindow.h"

#include <QDebug>
#include <QFileOpenEvent>

// http://doc.qt.nokia.com/qq/qq18-macfeatures.html
bool CApplication::event(QEvent *event)
{
    switch (event->type())
    {
        case QEvent::FileOpen:
            if (theMainWindow)
            {
                //theMainWindow->loadData(static_cast<QFileOpenEvent *>(event)->file(), "");
            }
            else
            {
                qWarning() << "main window is NULL, enqueueing event";
                filesToOpen.enqueue(static_cast<QFileOpenEvent *>(event)->file());
                startTimer(1000);
            }
            return true;
        default:
            return QApplication::event(event);
    }
}

void CApplication::timerEvent(QTimerEvent *event)
{
    if (theMainWindow)
    {
        while (!filesToOpen.empty())
        {
            //theMainWindow->loadData(filesToOpen.dequeue(), "");
        }
        killTimer(event->timerId());
    }
}
