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
    // Avrage IQ samples
    void averageIQ(int16_t *buffer, int len, int *avgI, int *avgQ);
    // Filters
    CWindowFunc *winfunc;
    // FIR lowpass filter
    CFIR<int16_t> *filter;
    // FIR lowpass IQ filter
    CFIR<int16_t> *filterIQ;
    // Average
    int avgI;
    int avgQ;
};

#endif // CAM_H
