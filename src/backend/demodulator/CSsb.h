#ifndef CSSB_H
#define CSSB_H

#include <QObject>
#include <CDemodulatorBase.h>

class CSsb : public CDemodulatorBase
{
    Q_OBJECT
public:
    explicit CSsb(QObject *parent=0, CDemodulatorBase::Mode mode = CDemodulatorBase::eLSB);
    QString getName();
signals:

public slots:
    void doWork();

};

#endif // CSSB_H
