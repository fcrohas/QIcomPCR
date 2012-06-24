#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <QObject>


class CCommand : public QObject
{
    Q_OBJECT
public:
    explicit CCommand(QObject *parent = 0);

    // Power command
    void setPower(bool value);
    bool getPower();

    // Set frequency
    void setFrequency(uint value);

    // Set frequency modulation
    void setModulation(uint value);

    // Set Antenna
    void setRadio(uint value);

    // Set Filter
    void setFilter(uint value);

    // Set IFShift
    void setIFShift(uint value);

    // set Squelch
    void setSquelch(uint value);

    // set Automatic Gain Control Mode
    void setAutomaticGainControl(bool value);

    // set Noise Blanker
    void setNoiseBlanker(bool value);

    // set Voice Control
    void setVoiceControl(bool value);

    // set Radio mode
    void setRadioMode(uint value);

    // set Sound Output
    void setSoundVolume(uint value);
    void setSoundMute(bool value);

    // Enum
    enum radioSound {eSound1=0, eSound2=1};
    enum radioMode {eBoth=0, eSingle=1, eDiversity=2}; // 0=both, 1=single, 2=diversity
    enum radioA {eRadio1=0, eRadio2=1}; // 0,1 with 0 for antenna 1
    enum mode { eLSB=0, eUSB=1, eAM=2, eCW=3, eFM=5, eWFM=6}; // 00=LSB, 01=USB, 02=AM, 03=CW, 05=FM, 06=WFM
    enum filter {e28k=0, e6k=1, e15k=2, e50k=3, e230k=4}; // 00 =2.8k, 01 = 6k, 02 =15k, 03=50k, 04=230k
    enum agc {eAgcOff=0, eAgcOn=1};
    enum nb {eNBOff=0, eNBOn=1};
    enum vsc {eVSCOff=0, eVSCOn=1};

signals:
    void sendData(QString &value);
public slots:

private:
    // Radio currently working on
    uint radio; // enum radio

    // Radio settings per antenna
    struct settings_t {
        uint frequency; // eg. 0106500000 for 106.5 Mhz padding is needed
        uint mode; // enum mode
        uint modulation;
        uint filter; // enum filter
        uint ifshift; // 00=min deviation, FF=max deviation
        uint squelch; // 00=min deviation, FF=max deviation
        bool agc; // false=off ,true=on
        bool nb; // Noise blanker
        bool vsc;
    };
    settings_t *currentRadio;
    // List of radio antenna settings
    QList<settings_t*> *radioList;

    // Power state
    bool power;

    // Radiomode
    uint radiomode; // 0=both, 1=single, 2=diversity
};

#endif // CCOMMAND_H
