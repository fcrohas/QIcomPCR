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
#ifndef CDLGSETTINGS_H
#define CDLGSETTINGS_H

#include <QDialog>
#include <CSettings.h>

namespace Ui {
class CDlgSettings;
}

class CDlgSettings : public QDialog
{
    Q_OBJECT
    
public:
    explicit CDlgSettings(QWidget *parent = 0,CSettings *settings=0);
    ~CDlgSettings();
    void addInputSoundDevices(QHash<QString, int> value);
    void addOutputSoundDevices(QHash<QString, int> value);
    void initialize();

public slots:
    void accept();

private:
    Ui::CDlgSettings *ui;
    CSettings *settings;
};

#endif // CDLGSETTINGS_H
