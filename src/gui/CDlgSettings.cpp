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
#include "CDlgSettings.h"
#include "ui_CDlgSettings.h"

CDlgSettings::CDlgSettings(QWidget *parent, CSettings *settings) :
    QDialog(parent),
    ui(new Ui::CDlgSettings)
{
    ui->setupUi(this);
    this->settings = settings;
    layout()->setSizeConstraint( QLayout::SetFixedSize );
    setWindowFlags( Qt::Dialog | Qt::WindowTitleHint );
}

CDlgSettings::~CDlgSettings()
{
    delete ui;
}

void CDlgSettings::accept()
{
    // save datas
    CSettings::global *params = new CSettings::global();
    params->inputDevice = ui->cbSoundInput->currentText();
    params->outputDevice = ui->cbSoundOutput->currentText();
    params->samplerate = ui->cbSamplerate->currentText().toInt();
    params->soundBufferSize = ui->cbBufferSize->currentText().toInt();
    settings->setGlobal(params);
    delete params;
    done(QDialog::Accepted);
}

void CDlgSettings::addInputSoundDevices(QHash<QString, int> value)
{
    QHashIterator<QString, int> it(value);
    while (it.hasNext()) {
        it.next();
        ui->cbSoundInput->addItem(it.key());
    }
}

void CDlgSettings::addOutputSoundDevices(QHash<QString, int> value)
{
    QHashIterator<QString, int> it(value);
    while (it.hasNext()) {
        it.next();
        ui->cbSoundOutput->addItem(it.key());
    }
}

void CDlgSettings::initialize()
{
    // Restore settings on load
    CSettings::global *params = new CSettings::global();
    settings->getGlobal(params);
    if (ui->cbSoundInput->findText(params->inputDevice)>0)
        ui->cbSoundInput->setCurrentIndex(ui->cbSoundInput->findText(params->inputDevice));
    if (ui->cbSoundOutput->findText(params->outputDevice)>0)
        ui->cbSoundOutput->setCurrentIndex(ui->cbSoundOutput->findText(params->outputDevice));
    if (ui->cbSamplerate->findText(QString("%1").arg(params->samplerate))>0)
        ui->cbSamplerate->setCurrentIndex(ui->cbSamplerate->findText(QString("%1").arg(params->samplerate)));
    if (ui->cbBufferSize->findText(QString("%1").arg(params->soundBufferSize))>0)
        ui->cbBufferSize->setCurrentIndex(ui->cbBufferSize->findText(QString("%1").arg(params->soundBufferSize)));
    delete params;
}
