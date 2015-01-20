#ifndef CAM_H
#define CAM_H

#include <QObject>
#include <CDemodulatorBase.h>
#include "filter/CFir.h"
#include "filter/CWindowFunc.h"

class CAm : public CDemodulatorBase
{
    Q_OBJECT
public:
    explicit CAm(QObject *parent = 0, CDemodulatorBase::Mode mode = CDemodulatorBase::eAM);
    QString getName();
signals:

public slots:
    void doWork();
    void slotSetFilter(uint frequency);

private:
    // Filters
    CWindowFunc *winfunc;
    // FIR bandpass filter
    CFIR<int16_t> *filter;
};

#endif // CAM_H
