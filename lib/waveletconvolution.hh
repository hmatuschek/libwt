#ifndef __WT_WAVELETCONVOLUTION_HH__
#define __WT_WAVELETCONVOLUTION_HH__

#include <vector>
#include "convolution.hh"
#include "waveletanalysis.hh"

namespace wt {

/** Implements the convolution operation in the wavelet time-scale space. I.e. the convolution of a
 * time-scale function with the reproducing kernel of a wavelet pair will result into a projection of
 * that function onto the space of wavelet transformed.
 * @bug Not implemented yet.
 * @ingroup analysis */
class WaveletConvolution : public WaveletAnalysis
{
public:
  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  WaveletConvolution(const Wavelet &wavelet, const RVector &Scales);
  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  WaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales);
  /** Copy constructor. */
  WaveletConvolution(const WaveletConvolution &other);
  /** Destructor. */
  virtual ~WaveletConvolution();

  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out)
  {
  }

protected:
  void _init_convolution();

protected:
  /** The list of convolution filters applied for the convolution with the reproducing kernel. */
  std::vector<Convolution *> _reprodKernel;
};

}

#endif // __WT_WAVELETCONVOLUTION_HH__
