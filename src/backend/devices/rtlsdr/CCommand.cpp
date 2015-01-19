#include "CCommand.h"
#include <QtWidgets>

CCommand::CCommand(QObject *parent) :
    QObject(parent),
    m_device(NULL),
    polarity(0),
    reverse(0),
    sound(NULL),
    demoThread(NULL)
{
    // Initialize radio struct
    radioList = new QList<radio_t*>();
    currentRadio = new radio_t;
    // Add two radio entry in list
    radioList->append(new radio_t);
    radioList->append(new radio_t);

    // Init serial connection object
    m_device = new CRtlSdr(this);
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));

    /// Demodulator
    currentRadio->demodulator = NULL;

    // timer event display refresh
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(getSNR()));
    timer->start(1000); // 200ms ???
    // initialize status
    status.snr[0] = 0;
    status.snr[1] = 0;
}

void CCommand::setPower(bool value)
{
    if (value) {
        power = m_device->open();
        status.power = true;

    } else {
        if (power) {
            m_device->close();
            status.power = false;
        }
    }
    emit dataChanged(status);
}

bool CCommand::getPower()
{
    return status.power;
}

void CCommand::setFrequency(uint value)
{
    m_device->setFrequency(value);
    currentRadio->frequency = value;
}

uint CCommand::getFrequency()
{
    return m_device->getFrequency();
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
    currentRadio->demodulator = CDemodulatorFactory::Builder((CDemodulatorBase::Mode)value);
    if (currentRadio->demodulator != NULL) {
        // set current SDR sampling rate
        currentRadio->demodulator->setSampleRate(RTLSDR_SAMPLE_RATE);
        // Set demodulator to thread
        currentRadio->demodulator->setSoundDevice(sound);
        demoThread = new QThread();
        connect(currentRadio->demodulator, SIGNAL(finished()), demoThread, SLOT(quit()));
        connect(currentRadio->demodulator, SIGNAL(finished()), currentRadio->demodulator, SLOT(deleteLater()));
        connect(demoThread, SIGNAL(finished()), demoThread, SLOT(deleteLater()));
        //connect(m_device,SIGNAL(sigSampleRead(int16_t*,int)),this,SLOT(slotSamplesRead(int16_t*,int)));
        m_device->setDemodulator(currentRadio->demodulator);
        connect(this,SIGNAL(sigSetFilter(uint)),currentRadio->demodulator,SLOT(slotSetFilter(uint)));
        currentRadio->demodulator->moveToThread(demoThread);
        demoThread->start();
    }
}

void CCommand::setFilter(uint value)
{
    switch(value) {
        case e28k : value = 2800; break;
        case e6k : value = 6000; break;
        case e15k : value = 15000; break;
        case e50k : value = 50000; break;
        case e230k : value = 230000; break;
    }

    currentRadio->filter = value;
    // send new filter value
    currentRadio->demodulator->slotSetFilter(value);
}

void CCommand::setRadio(uint value)
{
    if (value < (uint)radioList->count()) {
        currentRadio = radioList->at(value);
        radio = value;
    }
}

void CCommand::setRadioMode(uint value)
{
    QString data;

    data = QString("J00%1").arg(value, 2, 16, QChar('0')).toUpper();
    radiomode = value;
    //qDebug() << "Radio Mode " << data << "\n";
    emit sendData(data);
}


void CCommand::setIFShift(uint value)
{
    QString data;

    data = (radio == 0) ? "J43%1" : "J63%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->ifshift = value;
    //qDebug() << "IFShift " << data << "\n";
    emit sendData(data);
}

void CCommand::setSquelch(uint value)
{
    QString data;

    data = (radio == 0) ? "J41%1" : "J61%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    //qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}

void CCommand::setToneSquelch(uint value)
{
    QString data;
     // 0x80 is reverse so 0x8a is reverse 88.5Hz
     // Tone scan use H601 then J510a h601 j510b ....
    data = (radio == 0) ? "J51%1" : "J71%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    //qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}

void CCommand::setDTCS(uint value)
{
    QString data;

    data = (radio == 0) ? "J52%1%2%3" : "J72%1%2%3";

    data = data.arg(polarity, 1, 16, QChar('0')).arg(reverse, 1, 16, QChar('0')).arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    //qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}


void CCommand::setAutomaticGainControl(bool value)
{
    m_device->setAgcControl(value);
}

void CCommand::setNoiseBlanker(bool value)
{
    QString data;
    data = (radio == 0) ? "J46%1" : "J66%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->nb = value;
    //qDebug() << "Noise Blanker " << data << "\n";
    emit sendData(data);
}

void CCommand::setVoiceControl(bool value)
{
    QString data;
    data = (radio == 0) ? "J50%1" : "J70%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->vsc = value;
    //qDebug() << "Voice Control " << data << "\n";
    emit sendData(data);
}

void CCommand::setSoundVolume(uint value)
{
    QString data;
    data = (radio == 0) ? "J40%1" : "J60%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    //qDebug() << "Sound Volume " << data << "\n";
    currentRadio->volume = value;
    emit sendData(data);
}

void CCommand::setSoundMute(bool value)
{
    QString data;
    data = QString("JA2%1").arg(!value, 2, 16, QChar('0')).toUpper();
    //qDebug() << "Sound Mute " << data << "\n";
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
            status.power = true;
            qDebug() << "Connected";
            emit dataChanged(status);
            break;
        }
        sleep(1);
        status.power = false;
        emit dataChanged(status);
        m_device->close();
        sleep(1);
        retry++;
    }
    return opened;
}

void CCommand::Close()
{
    // Power it off
    status.power = false;
    emit dataChanged(status);
    setPower(false);
}

void CCommand::Initialize()
{

}

void CCommand::slotReceivedData(QString value)
{
    //qDebug() << "received " << value;
    //emit dataChanged(value);
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

uint CCommand::getSoundVolume()
{
    return currentRadio->volume;
}

void CCommand::setSoundDevice(ISound *sound) {
    qDebug() << "Sound device is set to command controller";
    this->sound = sound;
}

void CCommand::slotSamplesRead(int16_t *buffer, int len) {

}

void CCommand::getSNR() {
    status.readCount = m_device->log_t.dataReceive;
    status.sendCount = m_device->log_t.dataSent;
    if (currentRadio->power) {
        status.snr[radio] = currentRadio->demodulator->mad(2);
        emit dataChanged(status);
    }
}

void CCommand::setRadio(CCommand::radio_t radio) {
    if (radio.agc != currentRadio->agc) setAutomaticGainControl(radio.agc);
    if (radio.nb != currentRadio->nb) setNoiseBlanker(radio.nb);
    if (radio.filter != currentRadio->filter) setFilter(radio.filter);
    if (radio.frequency != currentRadio->frequency) setFrequency(radio.frequency);
    if (radio.ifshift != currentRadio->ifshift) setIFShift(radio.ifshift);
    if (radio.modulation != radio.modulation) setModulation(radio.modulation);
    if (radio.antenna != currentRadio->antenna) setRadioMode(radio.antenna);
    if (radio.squelch != currentRadio->squelch) setSquelch(radio.squelch);
    if (radio.vsc != currentRadio->vsc) setVoiceControl(radio.vsc);
    if (radio.volume != currentRadio->volume) setSoundVolume(radio.volume);
    if (radio.power != currentRadio->power) setPower(radio.power);

}

void CCommand::setBandscope(CCommand::bandscope_t bandscope) {
    QString data("ME0000%1%2%3%4%5");
    bandscope.samplewidth = bandscope.width / bandscope.step;
    data = data.arg(currentRadio->antenna +1).arg(bandscope.samplewidth, 2, 16, QChar('0'))
                                     .arg(bandscope.refresh, 2, 16, QChar('0'))
                                     .arg(bandscope.power, 2, 16, QChar('0'))
                                     .arg(bandscope.step, 8, 10, QChar('0'));
    emit sendData(data);
}
