#ifndef __WT_FFT_HH__
#define __WT_FFT_HH__

#include "config.hh"

#ifdef WT_WITH_FFTW3
#include "fft_fftw3.hh"
#else
#include "fft_kiss.hh"
#endif // WT_WITH_FFTW3

#endif // __WT_FFT_HH__
