#ifndef __WAVELETTRANSFORM_HH__
#define __WAVELETTRANSFORM_HH__

#include "waveletanalysis.hh"
#include "convolution.hh"
#include "movingaverage.hh"
#include <vector>


namespace wt {

/** Implements a complex, continious wavelet transform (i.e. \cite Holschneider1998).
 * @ingroup analyses */
class WaveletTransform: public WaveletAnalysis
{
public:
  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, const RVector &scales, bool subSample=false);
  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales, bool subSample=false);
  /** Constructor from other wavelet analysis. */
  WaveletTransform(const WaveletAnalysis &other, bool subSample=false);
  /** Destructor. */
  virtual ~WaveletTransform();

  /** Performs the wavelet transform on the given @c signal and stores the result into the given
   * @c out matrix. The wavelet transformed for the j-th scale is stored in the j-th column
   * of the matrix, hence the matrix must have N rows and K colmums where K is the number of scales
   * and N is the number of samples in signal. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &signal, Eigen::DenseBase<oDerived> &out)
  {
    // signal length
    int N = signal.size();

    // Iterate over all convolution filters grouping wavelets with the same size
    std::vector<Convolution *>::iterator filters = _filterBank.begin();
    for (size_t j=0; filters != _filterBank.end(); j+=(*filters)->numKernels(), filters++)
    {
      // Get subsampling
      int M = (*filters)->subSampling();
      // Get number of kernels in group
      int K = (*filters)->numKernels();

      if (1 == M) {
        // w/o sub-sampling -> direct overlap-add convolution
        (*filters)->apply(signal, out.block(0, j, N, K).derived());
        // continue with next block
        continue;
      }

      /*
       * Perform convolution with sub-sampling
       */
      // Number of samples in the sub-sampled signal
      int n = WT_IDIV_CEIL(N,M);
      // allocate some working arrays
      CVector subsig(n);     // <- Will hold the sub-sampled signal
      CMatrix subres(n, K);  // <- Will hold the sub-sampled result
      // For every shift within the M-fold sub-sampling:
      for (int m=0; m<M; m++) {
        // subsample input signal
        for (int i=0; i<n; i++) {
          subsig[i] = ((i*M+m) < N) ? CScalar(signal[i*M+m]) : 0;
        }
        // Apply overlap-add convolution
        (*filters)->apply(subsig, subres.derived());
        // Store results into output buffer (interleaved rows)
        for (int i=0; i<n; i++) {
          if ( (i*M+m) < N ) {
            out.block(i*M+m, j, 1, K) = subres.row(i);
          }
        }
      }
    }
  }

protected:
  /** Actually initializes the transformation. */
  void init_trafo();

protected:
  /** If @c true, the sub-sampling of the input signal is allowed. */
  bool _subSample;
  /** The list of convolution filters applied for the wavelet transform. */
  std::vector< Convolution *> _filterBank;
};

}

#endif // __WAVELETTRANSFORM_HH__
