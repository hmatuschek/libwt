#include "wavelettransform.hh"
#include <algorithm>

using namespace wt;

WaveletTransform::WaveletTransform(const Wavelet &wavelet, const RVector &scales)
  : _wavelet(wavelet), _scales(scales), _filterBank()
{
  init_trafo();
}

WaveletTransform::WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales)
  : _wavelet(wavelet), _scales(Nscales), _filterBank()
{
  for (int i=0; i<Nscales; i++) { _scales(i) = scales[i]; }
  init_trafo();
}

void
WaveletTransform::init_trafo()
{
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  // Determine kernel size for every scale and round up to next integer for which the FFT can
  // be computed fast. Also group the resulting kernel lengths
  std::list< std::pair<size_t, std::list<double> > > kernelSizes;
  for (size_t j=0; j<_scales.size(); j++) {
    size_t kernelSize = FFT::roundUp(std::ceil(_scales[j]*_wavelet.timeWidth()));
    if (0 == kernelSizes.size()) {
      // If first scale -> add new kernel size group
      kernelSizes.push_back(
            std::pair<size_t, std::list<double> >(kernelSize, std::list<double>(1, _scales[j])) );
    } else if (kernelSizes.back().first == kernelSize) {
      // If kernel size matches the last group
      kernelSizes.back().second.push_back( _scales[j] );
    } else {
      // If kernel size does not match last group -> add new kernel size group
      kernelSizes.push_back(
            std::pair<size_t, std::list<double> >(kernelSize, std::list<double>(1, _scales[j])) );
    }
  }

  // Create a block-convolution for each kernel size
  std::list< std::pair<size_t, std::list<double> > >::iterator group = kernelSizes.begin();
  for (; group != kernelSizes.end(); group++) {
    size_t N = group->first;         // <- size of kernels
    size_t M = group->second.size(); // # of kernels
    // Allocate matrix of filter kernels (each column holds a kernel)
    CMatrix kernels(N, M);
    std::list<double>::iterator scale = group->second.begin();
    for (size_t j=0; scale != group->second.end(); scale++, j++) {
      for (size_t i=0; i<N; i++) {
        kernels(i,j) = _wavelet.eval( (i-double(N)/2)/(*scale) )/( *scale );
      }
    }
    _filterBank.push_back(new Convolution(kernels));
  }
}

WaveletTransform::~WaveletTransform() {
  std::list<Convolution *>::iterator filter = _filterBank.begin();
  for (; filter != _filterBank.end(); filter++) { delete *filter; }
}

