#ifndef CWINDOWFUNC_H
#define CWINDOWFUNC_H

#include <QObject>
#include <math.h>

class CWindowFunc : public QObject
{
    Q_OBJECT
public:
    explicit CWindowFunc(QObject *parent = 0);
    void hann();
    void hamming();
    void blackman();
    void blackmanharris();
    void rectangle();
    void init(int size);
    double get(int i);
    double * getWindow();
signals:
    
public slots:

private:
    double *win;
    int N;
    
};

#endif // CWINDOWFUNC_H
