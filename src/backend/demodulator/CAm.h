#ifndef CAM_H
#define CAM_H

#include <QObject>
#include <IDemodulator.h>

class CAm : public IDemodulator
{
    Q_OBJECT
public:
    explicit CAm(QObject *parent = 0, IDemodulator::Mode mode = IDemodulator::eAM);

signals:

public slots:

};

#endif // CAM_H
