#include "CCommand.h"
#include <QtGui>

CCommand::CCommand(QObject *parent) :
    QObject(parent),
    m_device(NULL),
    stepsize(100000),
    scopewidth(5000000)
{
    // Initialize radio struct
    radioList = new QList<settings_t*>();
    currentRadio = new settings_t;
    // Add two radio entry in list
    radioList->append(new settings_t);
    radioList->append(new settings_t);

    // Init serial connection object
    m_device = new CDevicePCR2500("/dev/ttyUSB0", "38400", this);
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));
    connect(this, SIGNAL(sendData(QString&)), this, SLOT(write(QString&)));

    // Initialize sample width
    samplewidth = scopewidth / stepsize;
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
    if (value < (uint)radioList->count()) {
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

void CCommand::setBaudRate(uint value)
{
    QString data;
    data = QString("G10%1").arg(value);
    emit sendData(data);
}

void CCommand::setUpdateMode(uint value)
{
    QString data;
    data = QString("G30%1").arg(value);
    emit sendData(data);
}

bool CCommand::Open()
{
    // Device is initialized with 9600 bauds ??? conection
    int retry = 0;
    bool opened = false;
    // If device open failed retry three times
    while ((retry < 3) && (!opened))
    {
        if (m_device->open()) {
            opened = true;
            qDebug() << "Connected";
            break;
        }
        sleep(1);
        m_device->close();
        sleep(1);
        retry++;
    }
    // If not opened cancel
    qDebug() << "Opened ? " << opened;
    if (opened == false) return false;

    // Try to power it on
    retry = 0;
    while (retry < 3) {
        // Try to power it on
        qDebug() << "setPower();";
        setPower(true);
        // If powered end loop
        if (getPower()) {
            qDebug() << "Power on is OK";
            qDebug() << "so change baudrate";
            sleep(3);
            qDebug() << "change it on computer side too";
            //m_device->setBaudRate("38400");
            setBaudRate(CCommand::b38400);
            getPower();
            sleep(10);
            // Baud rate is now 38400
            return true;
        }
        sleep(3);
        retry++;
    }
    setPower(false);
    return false;
}

void CCommand::Close()
{
    // Power it off
    setPower(false);
}

void CCommand::Initialize()
{
    // Set Baud rate
    setRadio(0);
    setSquelch(255);
    setSoundVolume(0);
    setRadio(1);
    setSoundVolume(0);
    setSquelch(255);
    /*
    dbgWin->slotSendSerial("G2?");
    dbgWin->slotSendSerial("G4?");
    dbgWin->slotSendSerial("GE?");
    dbgWin->slotSendSerial("GD?");
    dbgWin->slotSendSerial("GA0?");
    dbgWin->slotSendSerial("GA1?");
    dbgWin->slotSendSerial("GA2?");
    dbgWin->slotSendSerial("GF?");
    */
    sleep(1);
    setUpdateMode(CCommand::eUpdOn);
    /*  NOT NEEDED */
    //dbgWin->slotSendSerial("J730000");
    setRadio(0);
    // Noise blanker is off
    setNoiseBlanker(false);
    setRadio(1);
    setNoiseBlanker(false);

    // Init radio 0 Frequency;
    setRadio(0);
    setModulation(CCommand::eWFM);
    setFilter(CCommand::e230k);
    setFrequency(106500000);
    setSquelch(0);
    setVoiceControl(CCommand::eVSCOff);
    setIFShift(128);

    // Init radio 1 Frequency
    setRadio(1);
    setModulation(CCommand::eFM);
    setFilter(CCommand::e15k);
    setFrequency(145425000);
    setSquelch(0);
    setSoundVolume(0);
    setVoiceControl(CCommand::eVSCOff);
    setIFShift(128);

    /*  NOT NEEDED */

    /* Unknown
    dbgWin->slotSendSerial("J4200");
    dbgWin->slotSendSerial("J4700");
    dbgWin->slotSendSerial("J6700");


    dbgWin->slotSendSerial("JC400");
    dbgWin->slotSendSerial("J7100");
    dbgWin->slotSendSerial("J720000");
    dbgWin->slotSendSerial("JC000");
    */
    // Mute radio before restoring values
    setRadio(0);
    setSoundMute(true);
    setSoundVolume(0);

    /* Unknown
    dbgWin->slotSendSerial("J8001");
    dbgWin->slotSendSerial("J8100");
    dbgWin->slotSendSerial("J8200");
    dbgWin->slotSendSerial("J8300");
    dbgWin->slotSendSerial("JC500");
    */
    setRadio(0);
    setSquelch(255);
    setVoiceControl(CCommand::eVSCOff);
    setRadio(1);
    setSquelch(255);
    setVoiceControl(CCommand::eVSCOff);

    setRadio(0);
    setSoundVolume(0);
    setRadio(1);
    setSoundVolume(0);
    setSquelch(255);
    setRadioMode(CCommand::eBoth);
    //dbgWin->slotSendSerial("JB000");
    setRadio(1);
    setSquelch(255);
    setVoiceControl(CCommand::eVSCOff);

    setRadio(0);
    setSquelch(1);
    setVoiceControl(CCommand::eVSCOff);
    setRadio(1);
    setVoiceControl(CCommand::eVSCOff);
    setSquelch(1);

    setRadio(0);
    setSoundVolume(60);
    setSoundMute(false);
}

void CCommand::slotReceivedData(QString value)
{
    //qDebug() << "received " << value;
    emit dataChanged(value);
}

void CCommand::write(QString &data)
{
    m_device->write(data);
}

long CCommand::getReadCount()
{
    return m_device->log_t.dataReceive;
}

long CCommand::getSendCount()
{
    return m_device->log_t.dataSent;
}

void CCommand::setBandScope(radioA antenna, int refresh, bool power)
{
    QString data("ME0000%1%2%3%4%5");
    qDebug() << "Band scope change " << QString("%1").arg(samplewidth, 2, 16, QChar('0')) << " step size " << QString("%1").arg(stepsize, 8, 10, QChar('0'));
    data = data.arg(antenna+1).arg(samplewidth, 2, 16, QChar('0')).arg(refresh, 2, 16, QChar('0')).arg(power, 2, 16, QChar('0')).arg(stepsize, 8, 10, QChar('0'));
    scopepower = power;
    scoperefresh = refresh;
    emit sendData(data);
}

void CCommand::setBandScopeWidth(int value)
{
    scopewidth = value;
    samplewidth = scopewidth / stepsize;
    setBandScope((radioA)radio, scoperefresh, scopepower);
}

void CCommand::setBandScopeStep(int value)
{
    stepsize= value;
    setBandScope((radioA)radio, scoperefresh, scopepower);
}
