#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <QObject>
#include <QTimer>
#include <CRtlSdr.h>
#include <CDemodulatorBase.h>
#include <CDemodulatorFactory.h>
#include <CBandScopeWorker.h>
#include <QThread>
#ifndef WIN32
#include <unistd.h>
#endif
#ifdef WIN32
#define sleep Sleep
#endif


class CCommand : public QObject
{
    Q_OBJECT
public:
    explicit CCommand(QObject *parent = 0);
    ~CCommand();

    // Radio settings per antenna
    struct radio_t {
        radio_t() : power(false),antenna(1),modulation(6),demodulator(NULL) {}
        int antenna;
        int frequency; // eg. 0106500000 for 106.5 Mhz padding is needed
        int mode; // enum mode
        int modulation;
        int filter; // enum filter
        int ifshift; // 00=min deviation, FF=max deviation
        int squelch; // 00=min deviation, FF=max deviation
        int step;
        bool agc; // false=off ,true=on
        bool nb; // Noise blanker
        bool vsc;
        bool mute;
        bool power;
        int volume;
        CDemodulatorBase *demodulator;
    };

    struct bandscope_t {
        bandscope_t() : width(1024000),step(25000),power(false) {}
        int width;
        int step;
        int samplewidth;
        int refresh;
        bool power;
    };

    struct status_t {
        status_t() : power(false),readCount(0),sendCount(0) {}
        int snr[2];
        bool power;
        int readCount;
        int sendCount;
        int16_t *bandscope;
        int bandscopesize;
    };

    // Power command
    bool getPower();

    // Set frequency
    uint getFrequency();

    // Set frequency modulation
    uint getModulation();

    // Set Filter
    uint getFilter();

    // Set IFShift
    uint getIFShift();

    // set Squelch
    uint getSquelch();

    // Volume
    uint getSoundVolume();

    // Enum
    enum radioSound {eSound1=0, eSound2=1};
    enum radioMode {eBoth=0, eSingle=1, eDiversity=2}; // 0=both, 1=single, 2=diversity
    enum radioA {eRadio1=0, eRadio2=1}; // 0,1 with 0 for antenna 1
    enum mode { eLSB=0, eUSB=1, eAM=2, eCW=3, eUkn=4, eFM=5, eWFM=6}; // 00=LSB, 01=USB, 02=AM, 03=CW, 05=FM, 06=WFM
    enum filter {e28k=0, e6k=1, e15k=2, e50k=3, e230k=4}; // 00 =2.8k, 01 = 6k, 02 =15k, 03=50k, 04=230k
    enum agc {eAgcOff=0, eAgcOn=1};
    enum nb {eNBOff=0, eNBOn=1};
    enum vsc {eVSCOff=0, eVSCOn=1};
    enum baudrate {b300=0,b1200=1,b4800=2,b9600=3,b19200=4,b38400=5,b57600=6,b115200=7};
    enum updateMode {eUpdOff=0, eUpdOn=1, eUpdBinOn=2, eUpdBinOff=3};

signals:
    void sendData(QString &value);
    void dataChanged(CCommand::status_t status);
    void sigFilter(uint value);

public slots:
    // Radio slot
    void setRadio(CCommand::radio_t radio);

    // bandscope slot
    void setBandscope(CCommand::bandscope_t bandscope);

    // Power command
    void setPower(bool value);

    // Set frequency
    void setFrequency(uint value);

    // Set frequency modulation
    void setModulation(uint value);

    // set Automatic Gain Control Mode
    void setAutomaticGainControl(bool value);

    // set Noise Blanker
    void setNoiseBlanker(bool value);

    // Set Antenna
    void setRadio(uint value);

    // Set Filter
    void setFilter(uint value);

    // Set IFShift
    void setIFShift(uint value);

    // set Voice Control
    void setVoiceControl(bool value);

    // set Radio mode
    void setRadioMode(uint value);

    // set Sound Output
    void setSoundVolume(uint value);
    void setSoundMute(bool value);

    // set Squelch
    void setSquelch(uint value);

    // setBaud rate
    void setBaudRate(uint value);

    // Update Mode
    void setUpdateMode(uint value);

    // Initialize radio
    void Initialize();

    // Open radio connection
    bool Open();

    // Close radio connection
    void Close();

    // Write command
    void write(QString &data);

    // Data state
    long getReadCount();
    long getSendCount();

    // Bandscope function
    void setToneSquelch(uint value);
    void setDTCS(uint value);
    void setSoundDevice(ISound *sound);

private slots:
    void slotReceivedData(QString value);
    void slotSamplesRead(int16_t *buffer,int len);
    void getSNR();

private:

    // Radio currently working on
    int radio;
    radio_t *currentRadio;
    status_t status;
    bandscope_t bandscope;
    // List of radio antenna settings
    QList<radio_t*> *radioList;

    // Power state
    bool power;

    // Radiomode
    uint radiomode; // 0=both, 1=single, 2=diversity

    int polarity;
    int reverse;
    bool scopepower;
    // Device driver
    CRtlSdr * m_device;
    // Sound device
    ISound *sound;
    // Demodulator thread
    QThread *demoThread;
    // Timer event
    QTimer *timer;
    // Bandscope worker
    CBandScopeWorker *bandscopeW;
    // Bandscope Thread
    QThread *bandscopeT;
};

#endif // CCOMMAND_H
