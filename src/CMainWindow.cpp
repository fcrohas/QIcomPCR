/**********************************************************************************************
    Copyright (C) 2008-2009 Oliver Eichler oliver.eichler@gmx.de

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

#include "CMainWindow.h"
#include "CDevicePCR2500.h"
#include "version.h"

#include <QtGui>
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32
#include <io.h>
#endif

#include "config.h"

CMainWindow * theMainWindow = 0;

CMainWindow::CMainWindow()
{
    theMainWindow = this;
    setObjectName("MainWidget");
    setWindowTitle("QIcomPCR");

    // setup splitter views
    mainSplitter = new QSplitter(Qt::Horizontal,this);
    setCentralWidget(mainSplitter);

    leftSplitter = new QSplitter(Qt::Vertical,this);
    mainSplitter->addWidget(leftSplitter);

    rightSplitter = new QSplitter(Qt::Vertical,this);
    mainSplitter->addWidget(rightSplitter);

    setCentralWidget(mainSplitter);

    canvasTab = new QTabWidget(this);
    rightSplitter->addWidget(canvasTab);

    plainText = new QPlainTextEdit(this);
    canvasTab->addTab(plainText, tr("Debug"));
/*
    canvas = new CCanvas(this);
    canvasTab->addTab(canvas,tr("Map"));

    actionGroupProvider = new CMenus(this);

    megaMenu = new CMegaMenu(canvas);
    leftSplitter->addWidget(megaMenu);
*/
    tmpTabWidget = new QTabWidget(this);
    tmpTabWidget->setTabsClosable(true);
    tmpTabWidget->setTabPosition(QTabWidget::South);;
    tmpTabWidget->hide();
    rightSplitter->addWidget(tmpTabWidget);

    m_device = new CDevicePCR2500("/dev/ttyUSB0", "115200", this);
    connect(m_device, SIGNAL(sigLiveLog(const CLiveLog&)), &CLiveLogDB::self(), SLOT(slotLiveLog(const CLiveLog&)));

}

CMainWindow::~CMainWindow()
{

}

