#include "CCommand.h"
#include <QtWidgets>

CCommand::CCommand(QObject *parent) :
    QObject(parent),
    m_device(NULL),
    polarity(0),
    reverse(0),
    sound(NULL),
    demoThread(NULL),
    radio(1)
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
    bandscopeW = new CBandScopeWorker();
    bandscopeT = new QThread();
    m_device->setBandscope(bandscopeW);
    bandscopeW->moveToThread(bandscopeT);
}

CCommand::~CCommand() {
    bandscopeT->quit();
    bandscopeT->wait();
    delete bandscopeT;
    delete bandscopeW;
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

    if (currentRadio->demodulator != NULL) {
          timer->stop();
          demoThread->quit();
          disconnect(SIGNAL(sigFilter(uint)),currentRadio->demodulator);
          demoThread->wait();
          delete demoThread;
          //delete currentRadio->demodulator;
          currentRadio->demodulator = NULL;
    }

    //if (!status.power)
    //    return;
    qDebug() << "Build demodulator " << value << "\r\n";
    currentRadio->modulation = value;
    currentRadio->demodulator = CDemodulatorFactory::Builder((CDemodulatorBase::Mode)value);
    if (currentRadio->demodulator != NULL) {
        // set current SDR sampling rate
        currentRadio->demodulator->setSampleRate(RTLSDR_SAMPLE_RATE);
        // Set demodulator to thread
        currentRadio->demodulator->setSoundDevice(sound);
        m_device->setDemodulator(currentRadio->demodulator);
        demoThread = new QThread();
        //connect(currentRadio->demodulator, SIGNAL(finished()), demoThread, SLOT(quit()));
        connect(demoThread, SIGNAL(finished()), currentRadio->demodulator, SLOT(deleteLater()));
        connect(demoThread, SIGNAL(finished()), demoThread, SLOT(deleteLater()));
        connect(this,SIGNAL(sigFilter(uint)), currentRadio->demodulator,SLOT(slotSetFilter(uint)));
        currentRadio->demodulator->moveToThread(demoThread);
        demoThread->start();
        timer->start(1000); // 200ms ???
    }
}

void CCommand::setFilter(uint value)
{
    if (!status.power)
        return;

    switch(value) {
        case e28k : value = 2800; break;
        case e6k : value = 6000; break;
        case e15k : value = 15000; break;
        case e50k : value = 50000; break;
        case e230k : value = 230000; break;
    }

    currentRadio->filter = value;
    // send new filter value
    emit sigFilter(value);
}

void CCommand::setRadio(uint value)
{
    if (value < (uint)radioList->count()) {
        //currentRadio = radioList->at(value);
        currentRadio->antenna = value;
        radio = value;
    }
    //qDebug() << "Build Radio " << value << "\r\n";
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
    qDebug() << "Build ifshift " << value << "\r\n";
    //qDebug() << "IFShift " << data << "\n";
    emit sendData(data);
}

void CCommand::setSquelch(uint value)
{
    QString data;

    data = (radio == 0) ? "J41%1" : "J61%1";

    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    //qDebug() << "Build squelch " << value << "\r\n";
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
    qDebug() << "Build tone squelch " << value << "\r\n";
    //qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}

void CCommand::setDTCS(uint value)
{
    QString data;

    data = (radio == 0) ? "J52%1%2%3" : "J72%1%2%3";

    data = data.arg(polarity, 1, 16, QChar('0')).arg(reverse, 1, 16, QChar('0')).arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->squelch = value;
    qDebug() << "Build dtcs " << value << "\r\n";
    //qDebug() << "Squelch " << data << "\n";
    emit sendData(data);
}


void CCommand::setAutomaticGainControl(bool value)
{
    //qDebug() << "Build agc " << value << "\r\n";
    m_device->setAgcControl(value);
}

void CCommand::setNoiseBlanker(bool value)
{
    QString data;
    data = (radio == 0) ? "J46%1" : "J66%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->nb = value;
    qDebug() << "Build Noise blanker " << value << "\r\n";
    //qDebug() << "Noise Blanker " << data << "\n";
    emit sendData(data);
}

void CCommand::setVoiceControl(bool value)
{
    QString data;
    data = (radio == 0) ? "J50%1" : "J70%1";
    data = data.arg(value, 2, 16, QChar('0')).toUpper();
    currentRadio->vsc = value;
    qDebug() << "Build VSC " << value << "\r\n";
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
            Initialize();
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
    if (opened)
            Initialize();
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
    setModulation(CDemodulatorBase::eWFM);
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
    if (status.power) {
        //qDebug() << "radio=" << radio << " antenna=" << currentRadio->antenna << "\r\n";
        status.snr[currentRadio->antenna] = currentRadio->demodulator->mad(2);
        status.bandscope = bandscopeW->getBins();
        status.bandscopesize = bandscopeW->getSize();
        emit dataChanged(status);
    }
}

void CCommand::setRadio(CCommand::radio_t radio) {
    //qDebug() << "antenna=" << radio.antenna << " freq="<< radio.frequency <<"\r\n";
    if (radio.power != currentRadio->power) setPower(radio.power);
    if (radio.antenna != currentRadio->antenna) setRadio(radio.antenna);
    if (radio.modulation != currentRadio->modulation) setModulation(radio.modulation);
    if (radio.filter != currentRadio->filter) setFilter(radio.filter);
    if (radio.frequency != currentRadio->frequency) setFrequency(radio.frequency);
    if (radio.agc != currentRadio->agc) setAutomaticGainControl(radio.agc);
    if (radio.nb != currentRadio->nb) setNoiseBlanker(radio.nb);
    if (radio.ifshift != currentRadio->ifshift) setIFShift(radio.ifshift);
    if (radio.squelch != currentRadio->squelch) setSquelch(radio.squelch);
    if (radio.vsc != currentRadio->vsc) setVoiceControl(radio.vsc);
    if (radio.volume != currentRadio->volume) setSoundVolume(radio.volume);
}

void CCommand::setBandscope(CCommand::bandscope_t bandscope) {
    if (bandscope.power == true) {
        bandscopeW->update.lock();
        int size = bandscope.width / bandscope.step;
        if (size != this->status.bandscopesize) {
            status.bandscope = new int16_t[size];
            status.bandscopesize = size;
        }
        bandscopeW->setBandScope(size);
        bandscopeW->update.unlock();
    }
}
