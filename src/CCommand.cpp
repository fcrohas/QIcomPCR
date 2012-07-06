#include "CCommand.h"
#include <QtGui>

CCommand::CCommand(QObject *parent) :
    QObject(parent)
{
    // Initialize radio struct
    radioList = new QList<settings_t*>();
    currentRadio = new settings_t;
    // Add two radio entry in list
    radioList->append(new settings_t);
    radioList->append(new settings_t);

}

void CCommand::setPower(bool value)
{
    QString data("H10%1");
    data = data.arg(value);
    power = value;
    qDebug() << "Power " << data << "\n";
    emit sendData(data);
}

bool CCommand::getPower()
{
    QString data("H1?");
    emit sendData(data);
    qDebug() << "Power ? " << data << "\n";
    return power;
}

void CCommand::setFrequency(uint value)
{
    QString data("K%1%2%3%4000");
    QString frequency;

    currentRadio->frequency = value;

    frequency = QString("%1").arg(value, 10, 10, QChar('0')).toUpper();

    data = data.arg(radio).arg(frequency).arg(currentRadio->modulation,2,16,QChar('0')).arg(currentRadio->filter,2,16,QChar('0'));
    qDebug() << "Frequency " << data << "\n";
    emit sendData(data);

}

uint CCommand::getFrequency()
{
    return currentRadio->frequency;
}

uint CCommand::getFilter()
{
    return currentRadio->filter;
}

uint CCommand::getIFShift()
{
    return currentRadio->ifshift;
}

uint CCommand::getModulation()
{
    return currentRadio->modulation;
}

uint CCommand::getSquelch()
{
    return currentRadio->squelch;
}

// At this time, only set to device on frequency set
void CCommand::setModulation(uint value)
{
    currentRadio->modulation = value;
    // Once modulation is set call back frequency
    setFrequency(currentRadio->frequency);
}

void CCommand::setFilter(uint value)
{
    currentRadio->filter = value;
    // Once filter is set call back frequency
    setFrequency(currentRadio->frequency);
}

void CCommand::setRadio(uint value)
{
    if (value < radioList->count()) {
        currentRadio = radioList->at(value);
        qDebug() << "dump radio " << value << " filter is " << currentRadio->filter;
        qDebug() << "dump radio " << value << " frequency is " << currentRadio->frequency;
        qDebug() << "dump radio " << value << " mode is " << currentRadio->modulation;
        radio = value;
    }
}

void CCommand::setRadioMode(uint value)
{
    QString data;

    data = QString("J00%1").arg(value, 2, 16, QChar('0')).toUpper();
    radiomode = value;
    qDebug() << "Radio Mode " << data << "\n";
    emit sendData(data);
}


void CCommand::setIFShift(uint value)
{
    QString data;

    data = (radio == 0) ? "J43%1" : "J63%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->ifshift = value;
    qDebug() << "IFShift " << data << "\n";
    emit sendData(data);
}

void CCommand::setSquelch(uint value)
{
    QString data;

    data = (radio == 0) ? "J41%1" : "J61%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}

void CCommand::setAutomaticGainControl(bool value)
{
    QString data;
    data = (radio == 0) ? "J45%1" : "J65%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->agc= value;
    qDebug() << "AGC " << data << "\n";
    emit sendData(data);
}

void CCommand::setNoiseBlanker(bool value)
{
    QString data;
    data = (radio == 0) ? "J46%1" : "J66%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->nb = value;
    qDebug() << "Noise Blanker " << data << "\n";
    emit sendData(data);
}

void CCommand::setVoiceControl(bool value)
{
    QString data;
    data = (radio == 0) ? "J50%1" : "J70%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->vsc = value;
    qDebug() << "Voice Control " << data << "\n";
    emit sendData(data);
}

void CCommand::setSoundVolume(uint value)
{
    QString data;
    data = (radio == 0) ? "J40%1" : "J60%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    qDebug() << "Sound Volume " << data << "\n";
    emit sendData(data);
}

void CCommand::setSoundMute(bool value)
{
    QString data;
    data = QString("JA2%1").arg(!value, 2, 16, QChar('0')).toUpper();
    qDebug() << "Sound Mute " << data << "\n";
    emit sendData(data);
}
