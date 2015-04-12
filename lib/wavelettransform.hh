#ifndef __WAVELETTRANSFORM_HH__
#define __WAVELETTRANSFORM_HH__

#include "waveletanalysis.hh"
#include "convolution.hh"
#include <list>


namespace wt {

/** Implements a complex, continious wavelet transform (i.e. \cite Holschneider1998).
 * @ingroup analyses */
class WaveletTransform: public WaveletAnalysis
{
public:
  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, const RVector &scales);
  /** Constructs a wavelet transform from the given @c wavelet at the specified @c scales. */
  WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales);
  /** Constructor from other wavelet analysis. */
  WaveletTransform(const WaveletAnalysis &other);
  /** Destructor. */
  virtual ~WaveletTransform();

  /** Performs the wavelet transform on the given @c signal and stores the result into the given
   * @c out matrix. The wavelet transformed for the j-th scale is stored in the j-th column
   * of the matrix, hence the matrix must have N rows and K colmums where K is the number of scales
   * and N is the number of samples in signal. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &signal, Eigen::DenseBase<oDerived> &out) {
    // Iterate over all convolution filters grouping wavelets with the same size
    std::list<Convolution *>::iterator filter = _filterBank.begin();
    for (size_t j=0; filter != _filterBank.end(); j+=(*filter)->numKernels(), filter++) {
      (*filter)->apply(signal, out.block(0, j, signal.size(), (*filter)->numKernels()).derived());
    }
  }

protected:
  /** Actually initializes the transformation. */
  void init_trafo();

protected:
  /** The list of convolution filters applied for the wavelet transform. */
  std::list<Convolution *> _filterBank;
};

}

#endif // __WAVELETTRANSFORM_HH__
