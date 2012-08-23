#ifndef CACARS_H
#define CACARS_H

#include <QObject>

extern "C" {
    #include <libacarsd.h>
}

class CAcars : public QObject
{
    Q_OBJECT
public:
    explicit CAcars(QObject *parent = 0, int bufferSize = 16384, int sampleRate=22050, int passes=8);
    void decode(uchar *buffer, int size);
signals:
    void sendData(QString data);
    
public slots:

private:
    libacarsd *L;
    
};

#endif // CACARS_H
