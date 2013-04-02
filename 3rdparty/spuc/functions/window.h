#include <spuc.h>
#include <complex.h>
namespace SPUC {
void hamming(double* w, long nf,double alpha, double beta);
void hanning(double* w, long nf);
void blackman(double* w, long nf);
void kaiser(double* w,long nf, double beta);
void chebc(double nf, double dp, double df, double n,double x0);
void cheby(double* w, long nf, long n, long ieo, double dp, double df, double x0);
}

