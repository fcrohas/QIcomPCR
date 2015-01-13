#ifndef CAM_H
#define CAM_H

#include <QObject>
#include <IDemodulator.h>
#include "CFir.h"
#include "CWindowFunc.h"

class CAm : public IDemodulator
{
    Q_OBJECT
public:
    explicit CAm(QObject *parent = 0, IDemodulator::Mode mode = IDemodulator::eAM);

signals:

public slots:
    void doWork();

private:
    // Filters
    CWindowFunc *winfunc;
    // FIR bandpass filter
    CFIR<int16_t> *filter;
};

#endif // CAM_H
