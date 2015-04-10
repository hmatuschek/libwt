#include "waveletsynthesis.hh"
#include <algorithm>

using namespace wt;

WaveletSynthesis::WaveletSynthesis(const Wavelet &wavelet, const RVector &scales)
  : _wavelet(wavelet), _scales(scales), _filterBank()
{
  init_trafo();
}

WaveletSynthesis::WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales)
  : _wavelet(wavelet), _scales(Nscales), _filterBank()
{
  for (int i=0; i<Nscales; i++) { _scales(i) = scales[i]; }
  init_trafo();
}

void
WaveletSynthesis::init_trafo()
{
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  _filterBank.clear();
  _filterBank.reserve(_scales.size());
  // Determine kernel size for every scale and round up to next integer for which the FFT can
  // be computed fast. Also group the resulting kernel lengths
  for (size_t j=0; j<_scales.size(); j++) {
    size_t N = FFT::roundUp(std::ceil(_scales[j]*_wavelet.timeWidth()));
    CVector kernel(N);
    for (size_t i=0; i<N; i++) {
      kernel(i) = (_wavelet.eval((i-double(N)/2)/_scales[j])/(_scales[j]*_scales[j]));
    }
    _filterBank.push_back(new Convolution(kernel));
  }
}

WaveletSynthesis::~WaveletSynthesis() {
  std::vector<Convolution *>::iterator filter = _filterBank.begin();
  for (; filter != _filterBank.end(); filter++) { delete *filter; }
}

