#ifndef CFM_H
#define CFM_H

#include <QObject>
#include <CDemodulatorBase.h>

class CFm : public CDemodulatorBase
{
    Q_OBJECT
public:
    explicit CFm(QObject *parent=0,CDemodulatorBase::Mode mode = CDemodulatorBase::eFM);
    QString getName();
signals:

public slots:
    void doWork();
    void slotSetFilter(uint frequency);

private:
    // previous real
    int16_t pre_real;
    // previous imaginary
    int16_t pre_img;
    // average for deemph filter
    int avg;
    // polar discriminant method
    int esbensen(int ar, int aj, int br, int bj);
    void multiply(int ar, int aj, int br, int bj, int *cr, int *cj);
    void deemph_filter(int16_t *buffer,int len);
    int fast_atan2(int y, int x);
    int polar_disc_fast(int ar, int aj, int br, int bj);
    int polar_discriminant(int ar, int aj, int br, int bj);
    // deemphasis coef
    int deemph_a;
    // Filters
    CWindowFunc *winfunc;
    // FIR bandpass filter
    CFIR<int16_t> *filter;
    // Init PLL
    void initPLL();
    // process PLL
    void  processPll(int i, int q);


};

#endif // CFM_H
