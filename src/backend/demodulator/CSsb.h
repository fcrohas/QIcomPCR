#ifndef CSSB_H
#define CSSB_H

#include <QObject>
#include <IDemodulator.h>

class CSsb : public IDemodulator
{
    Q_OBJECT
public:
    explicit CSsb(QObject *parent=0, IDemodulator::Mode mode = IDemodulator::eLSB);
signals:

public slots:
    void doWork();

};

#endif // CSSB_H
