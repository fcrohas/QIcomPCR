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

};

#endif // CFM_H
