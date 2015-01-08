#ifndef IDEMODULATOR_H
#define IDEMODULATOR_H

#include <QObject>

class IDemodulator : public QObject
{
    Q_OBJECT
public:
    enum Mode { eLSB=0, eUSB=1, eAM=2, eCW=3, eUkn=4, eFM=5, eWFM=6};
    explicit IDemodulator(QObject *parent = 0, IDemodulator::Mode mode = IDemodulator::eUkn);
signals:

public slots:
    void slotSamplesRead(int *buffer,int len);
    void slotSetFilter(uint frequency);

};

#endif // IDEMODULATOR_H
