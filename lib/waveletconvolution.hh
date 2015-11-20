#ifndef __WT_WAVELETCONVOLUTION_HH__
#define __WT_WAVELETCONVOLUTION_HH__

#include <vector>
#include "convolution.hh"
#include "waveletanalysis.hh"

namespace wt {

/** Implements the convolution operation in the wavelet time-scale space. I.e. the convolution of a
 * time-scale function with the reproducing kernel of a wavelet pair will result into a projection
 * of that function onto the space of wavelet transformed.
 * @bug Not implemented yet.
 * @ingroup analysis */
template <class Scalar>
class WaveletConvolution : public WaveletAnalysis
{
public:
  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  WaveletConvolution(const Wavelet &wavelet, const Eigen::VectorXd &Scales)
    : WaveletAnalysis(wavelet, scales), _reprodKernel()
  {
    _init_convolution();
  }

  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  WaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales)
    : WaveletAnalysis(wavelet, scales, Nscales), _reprodKernel()
  {
    _init_convolution();
  }

  /** Copy constructor. */
  WaveletConvolution(const WaveletAnalysis &other)
    : WaveletAnalysis(other), _reprodKernel()
  {
    _init_convolution();
  }

  /** Destructor. */
  virtual ~WaveletConvolution() {
    for (size_t i=0; i<_reprodKernel.size(); i++) {
      delete _reprodKernel[i];
    }
  }

  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out)
  {
  }

protected:
  void _init_convolution() {
    _reprodKernel.reserve(_scales.size());
    // For every location of the reproducting kernel ...
    for (int i=0; i<_scales.size(); i++) {
      // Scale of the rep. kern.
      double a = _scales[i];
      // Determine the approx. scale range the rep. kernel is supported on.

      // ...evaluate the kernel at every scale.
      for (int j=0; j<_scales.size(); j++) {
       /// @bug Implement.
      }
    }
  }

protected:
  /** The list of convolution filters applied for the convolution with the reproducing kernel. */
  std::vector<Convolution<Scalar> *> _reprodKernel;
};

}

#endif // __WT_WAVELETCONVOLUTION_HH__
