#include "CCommand.h"
#include <QtWidgets>

CCommand::CCommand(QObject *parent) :
    QObject(parent),
    m_device(NULL),
    stepsize(100000),
    scopewidth(5000000),
    polarity(0),
    reverse(0),
    sound(NULL),
    demoThread(NULL)
{
    // Initialize radio struct
    radioList = new QList<settings_t*>();
    currentRadio = new settings_t;
    // Add two radio entry in list
    radioList->append(new settings_t);
    radioList->append(new settings_t);

    // Init serial connection object
    m_device = new CRtlSdr(this);
    connect(m_device, SIGNAL(sigData(QString)), this, SLOT(slotReceivedData(QString)));

    // Initialize sample width
    samplewidth = scopewidth / stepsize;
    /// Demodulator
    currentRadio->demodulator = NULL;

    // timer event display refresh
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(getSNR()));
    timer->start(1000); // 200ms ???
}

void CCommand::setPower(bool value)
{
    if (value) {
        power = m_device->open();
        emit dataChanged("H101");
    } else {
        if (power) {
            m_device->close();
            power = false;
            emit dataChanged("H100");
        }
    }
}

bool CCommand::getPower()
{
    return power;
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
    /*if (currentRadio->demodulator != NULL) {
        //disconnect(SIGNAL(sigSetFilter(uint)), currentRadio->demodulator);
        if (demoThread!=NULL) {
            demoThread->terminate();
            delete demoThread;
        }
        //demo->update.unlock();
        delete currentRadio->demodulator;
        currentRadio->demodulator = NULL;
    }*/

    currentRadio->modulation = value;
    switch(value) {
        case eFM : currentRadio->demodulator = new CFm(NULL,IDemodulator::eFM); break;
        case eAM : currentRadio->demodulator = new CAm(NULL,IDemodulator::eAM); break;
        case eWFM : currentRadio->demodulator = new CFm(NULL,IDemodulator::eWFM); break;
        case eLSB : currentRadio->demodulator = new CSsb(NULL,IDemodulator::eLSB); break;
        case eUSB : currentRadio->demodulator = new CSsb(NULL,IDemodulator::eUSB); break;
    }
    if (currentRadio->demodulator != NULL) {
        // set current SDR sampling rate
        currentRadio->demodulator->setSampleRate(RTLSDR_SAMPLE_RATE);
        // Set demodulator to thread
        currentRadio->demodulator->setSoundDevice(sound);
        demoThread = new QThread();
        currentRadio->demodulator->moveToThread(demoThread);
        connect(currentRadio->demodulator, SIGNAL(finished()), demoThread, SLOT(quit()));
        connect(currentRadio->demodulator, SIGNAL(finished()), currentRadio->demodulator, SLOT(deleteLater()));
        connect(demoThread, SIGNAL(finished()), demoThread, SLOT(deleteLater()));
        //connect(m_device,SIGNAL(sigSampleRead(int16_t*,int)),this,SLOT(slotSamplesRead(int16_t*,int)));
        m_device->setDemodulator(currentRadio->demodulator);
        connect(this,SIGNAL(sigSetFilter(uint)),currentRadio->demodulator,SLOT(slotSetFilter(uint)));
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
    emit sigSetFilter(value);
    //demo->slotSetFilter(value);
    // Once filter is set call back frequency
    //setFrequency(currentRadio->frequency);
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
            qDebug() << "Connected";
            emit dataChanged("H101");
            break;
        }
        sleep(1);
        emit dataChanged("H100");
        m_device->close();
        sleep(1);
        retry++;
    }
    return opened;
}

void CCommand::Close()
{
    // Power it off
    emit dataChanged("H100");
    setPower(false);
}

void CCommand::Initialize()
{

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
    //qDebug() << "Band scope change " << QString("%1").arg(samplewidth, 2, 16, QChar('0')) << " step size " << QString("%1").arg(stepsize, 8, 10, QChar('0'));
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
    //qDebug() << "SNR=" << QString("I%1%2").arg((radio==0)?"1":"5").arg(demo->mad(2),2,16, QChar('0')) << "\r\n";
    emit dataChanged(QString("I%1%2").arg((radio==0)?"1":"5").arg(currentRadio->demodulator->mad(2),2,16, QChar('0')));
}
