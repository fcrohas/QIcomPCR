#ifndef CBANDSCOPEWORKER_H
#define CBANDSCOPEWORKER_H

#include <QThread>
#include <QMutex>
#include <math.h>
#include <fftw3.h>
#include <stdint.h>
#define DEFAULT_BUF_LENGTH		(1 * 16384)
#define MAXIMUM_OVERSAMPLE		16
#define MAXIMUM_BUF_LENGTH		(MAXIMUM_OVERSAMPLE * DEFAULT_BUF_LENGTH)

class CBandScopeWorker : public QObject
{
    Q_OBJECT
public:
    explicit CBandScopeWorker(QObject *parent = 0);
    ~CBandScopeWorker();
    void setBandScope(int bins);
    void setData(int16_t *buffer, int length);
    // Mutex locker
    QMutex update;
    // Get size
    int getSize();
    // get buufer
    int16_t* getBins();
signals:

public slots:
    void doWork();

private:
    int bins;
    int16_t *buffer;
    int length;
    int16_t *binsData;
    fftw_complex *fftSamplesIn;
    fftw_complex *fftSamplesOut;
    fftw_plan fftplan;
};

#endif // CBANDSCOPEWORKER_H
