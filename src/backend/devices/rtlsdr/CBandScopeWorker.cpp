#include "CBandScopeWorker.h"
#include <QDebug>

CBandScopeWorker::CBandScopeWorker(QObject *parent) :
    QObject(parent),
    bins(512),
    fftplan(NULL)
{
    fftSamplesIn = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * MAXIMUM_BUF_LENGTH);
    fftSamplesOut = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * MAXIMUM_BUF_LENGTH);
}

CBandScopeWorker::~CBandScopeWorker() {
    if (fftplan != NULL)
        fftw_destroy_plan(fftplan);
    fftw_free(fftSamplesIn);
    fftw_free(fftSamplesOut);
}

void CBandScopeWorker::doWork() {
    update.lock();
    for (int i =0; i < this->length;i+=2) {
        fftSamplesIn[i][0] = this->buffer[i]*1.0/32768.0 ; // to double
        fftSamplesIn[i][1] = this->buffer[i+1]*1.0/32768.0 ; // to double
    }
    fftw_execute(fftplan);
    // fill output buffer
    for (int i =0; i < this->bins;i++) {
        this->binsData[i] = sqrt(pow(fftSamplesOut[i][0],2) + pow(fftSamplesOut[i][1],2))*32768/255;
        //qDebug() << "data=" << this->binsData[i];
    }
    update.unlock();
}

void CBandScopeWorker::setBandScope(int bins) {
    if(bins != this->bins) {
        this->bins = bins;
        binsData = new int16_t[this->bins];
        if (fftplan!= NULL)
            fftw_destroy_plan(fftplan);
        fftplan = fftw_plan_dft_1d( this->bins*2, fftSamplesIn, fftSamplesOut, FFTW_FORWARD, FFTW_ESTIMATE );
    }
}

void CBandScopeWorker::setData(int16_t *buffer, int length) {
    this->buffer = buffer;
    this->length = length;
}

int CBandScopeWorker::getSize() {
    return this->bins;
}

int16_t* CBandScopeWorker::getBins() {
    return this->binsData;
}
