#ifndef CSETTINGS_H
#define CSETTINGS_H

#include <QObject>
#include <QSettings>
#include <CCommand.h>

class CSettings : public QSettings
{
    Q_OBJECT
public:
    explicit CSettings(QObject *parent = 0);
    void reload();
    void save();

    struct radio {
        int frequency;
        int step;
        int filter;
        int IF;
        int squelch;
        int mode;
        bool agc;
        bool vsc;
        bool nb;
        int volume;
    };

    struct sound {
        int volume;
        bool soundcard;
        bool activated;
    };

    struct global {
        QString inputDevice;
        QString outputDevice;
        int samplerate;
        int soundBufferSize;
        int fftSize;
    };

    radio getRadio(int value);
    void setRadio(int radionum, radio *value);

    sound getSound(int value);
    void setSound(int radionum, sound *value);

    void getGlobal(global *value);
    void setGlobal(global *value);
signals:
    
public slots:

private:
    void initialize();

};

#endif // CSETTINGS_H
