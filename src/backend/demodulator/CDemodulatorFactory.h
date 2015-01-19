#ifndef CDEMODULATORFACTORY_H
#define CDEMODULATORFACTORY_H

#include <QObject>
#include <CDemodulatorBase.h>
#include <CAm.h>
#include <CFm.h>
#include <CSsb.h>

class CDemodulatorFactory : public QObject
{
    Q_OBJECT
public:
    // Demodulator factory
    static CDemodulatorBase* Builder(CDemodulatorBase::Mode mode);

signals:

public slots:

};

#endif // CDEMODULATORFACTORY_H
