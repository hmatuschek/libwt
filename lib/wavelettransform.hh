#ifndef __WAVELETTRANSFORM_HH__
#define __WAVELETTRANSFORM_HH__

#include "waveletanalysis.hh"
#include "convolution.hh"
#include <vector>
#include <list>


namespace wt {

/** Implements a complex, continious wavelet transform (i.e. \cite Holschneider1998).
 * @ingroup analyses */
template <class Scalar>
class WaveletTransform: public WaveletAnalysis
{
public:
  typedef typename Traits<Scalar>::Complex CScalar;
  typedef typename Traits<Scalar>::RVector RVector;
  typedef typename Traits<Scalar>::CVector CVector;
  typedef typename Traits<Scalar>::CMatrix CMatrix;

public:
  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, const RVector &scales, bool subSample=false)
    : WaveletAnalysis(wavelet, scales), _subSample(subSample), _filterBank()
  {
    init_trafo();
  }

  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales, bool subSample=false)
    : WaveletAnalysis(wavelet, scales, Nscales), _subSample(subSample), _filterBank()
  {
    init_trafo();
  }

  /** Constructor from other wavelet analysis. */
  WaveletTransform(const WaveletAnalysis &other, bool subSample=false)
    : WaveletAnalysis(other), _subSample(subSample), _filterBank()
  {
    init_trafo();
  }

  /** Destructor. */
  virtual ~WaveletTransform() {
    // Free filter bank
    typename std::vector<Convolution<Scalar> *>::iterator filter = _filterBank.begin();
    for (; filter != _filterBank.end(); filter++) { delete *filter; }
  }

  /** Performs the wavelet transform on the given @c signal and stores the result into the given
   * @c out matrix. The wavelet transformed for the j-th scale is stored in the j-th column
   * of the matrix, hence the matrix must have N rows and K colmums where K is the number of scales
   * and N is the number of samples in signal. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &signal, Eigen::DenseBase<oDerived> &out)
  {
    // signal length
    int N = signal.size();
    // Get start indices for each transformation block
    std::vector<size_t> blockIdxs(_filterBank.size());
    for (size_t j=0; j<_filterBank.size(); j++) {
      if (0 == j) {
        blockIdxs[0] = 0;
      } else {
        blockIdxs[j] = blockIdxs[j-1] + _filterBank[j-1]->numKernels();
      }
    }

    // Iterate over all convolution filters grouping wavelets with the same size
    #pragma omp parallel for
    for (size_t j=0; j<_filterBank.size(); j++)
    {
      // Get convolution filters
      Convolution<Scalar> *filters = _filterBank[j];
      // Get subsampling
      int M = filters->subSampling();
      // Get start column in output matrix
      size_t outCol = blockIdxs[j];
      // Get number of kernels in group / # columns in out
      int K = filters->numKernels();

      if (1 == M) {
        // w/o sub-sampling -> direct overlap-add convolution
        filters->apply(signal, out.block(0, outCol, N, K).derived());
        // continue with next block
        continue;
      }

      /*
       * Perform convolution with sub-sampling
       */
      // Number of samples in the sub-sampled signal
      int n = WT_IDIV_CEIL(N,M);
      // subsample input signal
      CVector subsig(n);
      for (int i=0; i<n; i++) {
        int mmax = std::min(N-i*M, M);
        subsig[i] = signal.segment(i*M, mmax).sum();
      }

      // Apply overlap-add convolution
      CMatrix subres(n, K);  // <- Will hold the sub-sampled result
      filters->apply(subsig, subres.derived());

      // Interpolate results into output buffer
      for (int i=0; i<n; i++) {
        int mmax = std::min(N-i*M, M);
        // For every shift within the M-fold sub-sampling:
        for (int m=0; m<mmax; m++) {
          out.block(i*M+m, outCol, 1, K) = (subres.row(i)*(M-m))/M;
          if ((i+1)<n) { out.block(i*M+m, outCol, 1, K) += (subres.row(i+1)*m)/M; }
        }
      }
    }
  }

protected:
  /** Actually initializes the transformation. */
  void init_trafo()
  {
    // Sort scales (ascending order)
    std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

    // Determine kernel size for every scale and round up to next integer for which the FFT can
    // be computed fast. Also group the resulting kernels by (rounded) size. This allows to perform
    // the forward FFT of the signal only once for each group
    std::list< std::pair<size_t, std::list<double> > > kernelSizes;
    for (int j=0; j<_scales.size(); j++) {
      // Get the "kernel size" in samples, round up to the next integer for which the
      // convolution can be performed fast.
      size_t kernelSize = FFT<Scalar>::roundUp(std::ceil(_scales[j]*2*_wavelet.cutOffTime()));
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
      _filterBank.push_back(new Convolution<Scalar>(kernels, M));
    }
  }


protected:
  /** If @c true, the sub-sampling of the input signal is allowed. */
  bool _subSample;
  /** The list of convolution filters applied for the wavelet transform. */
  std::vector<Convolution<Scalar> *> _filterBank;
};

}

#endif // __WAVELETTRANSFORM_HH__
