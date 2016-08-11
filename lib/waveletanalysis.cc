#include "waveletanalysis.hh"

using namespace wt;

WaveletAnalysis::WaveletAnalysis(const Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXd> &scales)
  : _wavelet(wavelet), _scales(scales)
{
  // pass...
}

WaveletAnalysis::WaveletAnalysis(const Wavelet &wavelet, double *scales, int Nscales)
  : _wavelet(wavelet), _scales(Nscales)
{
  for (int i=0; i<Nscales; i++) {
    _scales(i) = scales[i];
  }
}

WaveletAnalysis::WaveletAnalysis(const WaveletAnalysis &other)
  : _wavelet(other._wavelet), _scales(other._scales)
{
  // pass...
}

WaveletAnalysis::~WaveletAnalysis() {
  // pass...
}
