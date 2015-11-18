#include "wavelettransform.hh"
#include <algorithm>
#include <iostream>
#include <list>


using namespace wt;

WaveletTransform::WaveletTransform(const Wavelet &wavelet, const RVector &scales, bool subSample)
  : WaveletAnalysis(wavelet, scales), _subSample(subSample), _filterBank()
{
  init_trafo();
}

WaveletTransform::WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales, bool subSample)
  : WaveletAnalysis(wavelet, scales, Nscales), _subSample(subSample), _filterBank()
{
  init_trafo();
}

WaveletTransform::WaveletTransform(const WaveletAnalysis &other, bool subSample)
  : WaveletAnalysis(other), _subSample(subSample), _filterBank()
{
  init_trafo();
}

void
WaveletTransform::init_trafo()
{
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  // Determine kernel size for every scale and round up to next integer for which the FFT can
  // be computed fast. Also group the resulting kernels by (rounded) size. This allows to perform
  // the forward FFT only once for each group
  std::list< std::pair<size_t, std::list<double> > > kernelSizes;
  for (int j=0; j<_scales.size(); j++) {
    // Get the "kernel size" in samples, round up to the next integer for which the
    // convolution can be performed fast.
    size_t kernelSize = FFT::roundUp(std::ceil(_scales[j]*2*_wavelet.cutOffTime()));
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
    // size of kernels
    size_t N = group->first;
    // # of kernels
    size_t K = group->second.size();
    // Smallest scale in group of kernels
    double minScale  = group->second.front();
    // highest frequency in group of kernels
    double maxFreq = (1+_wavelet.cutOffFreq())/minScale;
    // possible sub-sampling for all kernels in group
    size_t M = std::max(1, int(0.5/maxFreq));
    // if sub-sampling is disabled -> M = 1
    if (!_subSample) { M = 1; }
    // re-evaluate kernel-size (round up to a multiple of sub-sampling)
    N = M * WT_IDIV_CEIL(N,M);
    // Allocate matrix of filter kernels (each column holds a kernel)
    CMatrix kernels(N/M, K);
    // Evaluate (subsampled) kernels
    std::list<double>::iterator scale = group->second.begin();
    for (size_t j=0; scale != group->second.end(); scale++, j++) {
      for (size_t i=0; i<N/M; i++) {
        kernels(i,j) = _wavelet.evalAnalysis( M*(i-double(N/M)/2)/(*scale) )/( *scale );
      }
    }
    // Store filter together with sub-sampling
    _filterBank.push_back(new Convolution(kernels, M));
  }
}

WaveletTransform::~WaveletTransform() {
  // Free filter bank
  std::vector<Convolution *>::iterator filter = _filterBank.begin();
  for (; filter != _filterBank.end(); filter++) { delete *filter; }
}

