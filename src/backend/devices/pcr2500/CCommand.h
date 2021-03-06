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
#ifndef CCOMMAND_H
#define CCOMMAND_H

#include <QObject>
#include <CDevicePCR2500.h>
#ifdef WIN32
#include <unistd.h>
#define sleep Sleep
#endif


class CCommand : public QObject
{
    Q_OBJECT
public:
    explicit CCommand(QObject *parent = 0);

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
    void dataChanged(QString value);

public slots:
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
    void setBandScope(radioA antenna,int refresh, bool power);
    void setBandScopeWidth(int value);
    void setBandScopeStep(int value);
    void setToneSquelch(uint value);
    void setDTCS(uint value);

private slots:
    void slotReceivedData(QString value);

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
        uint volume;
    };
    settings_t *currentRadio;
    // List of radio antenna settings
    QList<settings_t*> *radioList;

    // Power state
    bool power;

    // Radiomode
    uint radiomode; // 0=both, 1=single, 2=diversity

    // BandScope
    int samplewidth;
    int scopewidth;
    int stepsize;
    int scoperefresh;
    int polarity;
    int reverse;
    bool scopepower;

    // Device driver
    CDevicePCR2500 * m_device;

};

#endif // CCOMMAND_H
