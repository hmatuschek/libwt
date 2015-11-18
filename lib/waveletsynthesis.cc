#include "waveletsynthesis.hh"
#include <algorithm>

using namespace wt;

WaveletSynthesis::WaveletSynthesis(const Wavelet &wavelet, const RVector &scales)
  : WaveletAnalysis(wavelet, scales), _filterBank()
{
  init_synthesis();
}

WaveletSynthesis::WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales)
  : WaveletAnalysis(wavelet, scales, Nscales), _filterBank()
{
  init_synthesis();
}

WaveletSynthesis::WaveletSynthesis(const WaveletAnalysis &other)
  : WaveletAnalysis(other), _filterBank()
{
  init_synthesis();
}

void
WaveletSynthesis::init_synthesis()
{
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  _filterBank.clear();
  _filterBank.reserve(_scales.size());
  // Determine kernel size for every scale and round up to next integer for which the FFT can
  // be computed fast. Also group the resulting kernel lengths
  for (int j=0; j<_scales.size(); j++) {
    size_t N = FFT::roundUp(std::ceil(_scales[j]*2*_wavelet.cutOffTime()));
    CVector kernel(N);
    for (size_t i=0; i<N; i++) {
      kernel(i) = (_wavelet.evalSynthesis((i-double(N)/2)/_scales[j])/(_scales[j]*_scales[j]));
    }
    _filterBank.push_back(new Convolution(kernel));
  }
}

WaveletSynthesis::~WaveletSynthesis() {
  std::vector<Convolution *>::iterator filter = _filterBank.begin();
  for (; filter != _filterBank.end(); filter++) { delete *filter; }
}

