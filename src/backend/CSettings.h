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
    };

    struct sound {
        int volume;
        bool soundcard;
        bool activated;
    };

    radio getRadio(int value);
    void setRadio(int radionum, radio *value);

    sound getSound(int value);
    void setSound(int radionum, sound *value);

signals:
    
public slots:

private:
    void initialize();

};

#endif // CSETTINGS_H
