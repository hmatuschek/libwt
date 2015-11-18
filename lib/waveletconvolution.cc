#include "waveletconvolution.hh"

using namespace wt;

WaveletConvolution::WaveletConvolution(const Wavelet &wavelet, const RVector &scales)
  : WaveletAnalysis(wavelet, scales), _reprodKernel()
{
  _init_convolution();
}

WaveletConvolution::WaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales)
  : WaveletAnalysis(wavelet, scales, Nscales), _reprodKernel()
{
  _init_convolution();
}

WaveletConvolution::WaveletConvolution(const WaveletConvolution &other)
  : WaveletAnalysis(other), _reprodKernel()
{
  _init_convolution();
}

WaveletConvolution::~WaveletConvolution() {
  for (size_t i=0; i<_reprodKernel.size(); i++) {
    delete _reprodKernel[i];
  }
}

void
WaveletConvolution::_init_convolution() {
  _reprodKernel.reserve(_scales.size());
  // For every location of the reproducting kernel ...
  for (size_t i=0; i<_scales.size(); i++) {
    // Scale of the rep. kern.
    double a = _scales[i];
    // Determine the approx. scale range the rep. kernel is supported on.

    // ...evaluate the kernel at every scale.
    for (size_t j=0; j<_scales.size(); j++) {

    }
  }
}



