#ifndef CFM_H
#define CFM_H

#include <QObject>
#include <IDemodulator.h>

class CFm : public IDemodulator
{
    Q_OBJECT
public:
    explicit CFm(QObject *parent=0,IDemodulator::Mode mode = IDemodulator::eFM);

signals:

public slots:
    void slotSamplesRead(int16_t *buffer,int len);

};

#endif // CFM_H
