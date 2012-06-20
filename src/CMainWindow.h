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
#include <QPlainTextEdit>
class QSplitter;
class QLabel;
class QTabWidget;
class QComboBox;
class QSocketNotifier;
class QAction;
class QCheckBox;
class IDevice;
class CMainWindow : public QMainWindow
{
    Q_OBJECT;
    public:
        CMainWindow();
        virtual ~CMainWindow();

        QTabWidget * getCanvasTab(){return canvasTab;}

    protected:

    private slots:

    private:
        /// horizontal main splitter holding the canvas and the tool view
        QSplitter * mainSplitter;
        /// the vertical splitter holding the tool views
        QSplitter * leftSplitter;
        /// the vertical splitter holding the canvas and track info view
        QSplitter * rightSplitter;

        QTabWidget * tmpTabWidget;

        /// left hand tool box
        QTabWidget  * tabbar;

        QTabWidget * canvasTab;

        QPlainTextEdit * plainText;

        IDevice * m_device;

};

extern CMainWindow * theMainWindow;
#endif                           //CMAINWINDOW_H
