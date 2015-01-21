#include "CBandScopeWorker.h"

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
        fftSamplesIn[i][0] = this->buffer[i] ;
        fftSamplesIn[i][1] = this->buffer[i+1] ;
    }
    fftw_execute(fftplan);
    // fill output buffer
    for (int i =0; i < this->bins;i++) {
        this->binsData[i] = sqrt(fftSamplesOut[i][0] * fftSamplesOut[i][0] + fftSamplesOut[i][1] * fftSamplesOut[i][1]);
    }
    update.unlock();
}

void CBandScopeWorker::setBandScope(int bins) {
    if(bins != this->bins) {
        this->bins = bins;
        binsData = new int16_t[bins];
        if (fftplan!= NULL)
            fftw_destroy_plan(fftplan);
        fftplan = fftw_plan_dft_1d( bins, fftSamplesIn, fftSamplesOut, FFTW_FORWARD, FFTW_ESTIMATE );
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
