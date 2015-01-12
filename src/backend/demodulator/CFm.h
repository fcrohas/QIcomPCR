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
    void doWork();

private:
    // Filters
    CWindowFunc *winfunc;
    // FIR bandpass filter
    CFIR<int16_t> *filter;
    // previous real
    int16_t pre_real;
    // previous imaginary
    int16_t pre_img;
    // polar discriminant method
    int esbensen(int ar, int aj, int br, int bj);
    void multiply(int ar, int aj, int br, int bj, int *cr, int *cj);
    int fast_atan2(int y, int x);
    int polar_disc_fast(int ar, int aj, int br, int bj);
};

#endif // CFM_H
