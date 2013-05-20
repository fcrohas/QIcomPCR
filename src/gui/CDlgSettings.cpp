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
