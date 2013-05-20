#include "CSettings.h"

CSettings::CSettings(QObject *parent) :
    QSettings("Lilisoft.com","QIcomPcr",parent)
{

}

CSettings::radio CSettings::getRadio(int value)
{
    radio current;
    beginGroup(QString("Radio/Antenna%1").arg(value));
    current.frequency = this->value("Frequency", 106500000).toInt(); // RTS FM
    current.filter = this->value("Filter", CCommand::e230k).toInt(); // WFM Filter
    current.step = this->value("Step", 50000).toInt(); // WFM is 50Khz step
    current.IF = this->value("IF", 128).toInt(); // Middle by default
    current.squelch = this->value("Squelch", 0).toInt(); // Middle by default
    current.nb = this->value("NoiseBlanker", CCommand::eNBOff).toBool();
    current.agc = this->value("AGC", CCommand::eAgcOff).toBool();
    current.vsc = this->value("VSC", CCommand::eVSCOff).toBool();
    current.mode = this->value("Mode", CCommand::eWFM).toInt();
    endGroup();
    return current;
}

void CSettings::setRadio(int radionum,CSettings::radio *value)
{
    beginGroup(QString("Radio/Antenna%1").arg(radionum));
    this->setValue("Frequency",value->frequency);
    this->setValue("Filter",value->filter);
    this->setValue("Step",value->step);
    this->setValue("IF", value->IF);
    this->setValue("Squelch",value->squelch);
    this->setValue("NoiseBlanker",value->nb);
    this->setValue("AGC",value->agc);
    this->setValue("VSC",value->vsc);
    this->setValue("Mode",value->mode);
    endGroup();
}

CSettings::sound CSettings::getSound(int value)
{
    sound current;
    beginGroup(QString("Sound/Channel%1").arg(value));
    current.volume = this->value("Volume", 15).toInt(); // Volume
    current.soundcard = this->value("SoundOnPC", false).toBool(); // Volume
    current.activated = this->value("Activated", true).toBool(); // Volume
    endGroup();
}

void CSettings::setSound(int radionum, CSettings::sound *value)
{
    beginGroup(QString("Sound/Channel%1").arg(radionum));
    this->setValue("Volume",value->volume);
    this->setValue("SoundOnPC",value->soundcard);
    this->setValue("Activated",value->activated);
    endGroup();

}
