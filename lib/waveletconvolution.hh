#ifndef __WT_WAVELETCONVOLUTION_HH__
#define __WT_WAVELETCONVOLUTION_HH__

#include <vector>
#include "convolution.hh"
#include "waveletanalysis.hh"


namespace wt {

/** Implements the convolution operation in the wavelet time-scale space. That is, the
 * convolution of a time-scale function with the reproducing kernel of a wavelet pair.
 * @bug Not implemented yet.
 * @ingroup analysis */
template <class Scalar>
class GenericWaveletConvolution : public WaveletAnalysis
{
public:
  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  GenericWaveletConvolution(const Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXd> &Scales);

  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  GenericWaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales);

  /** Copy constructor. */
  GenericWaveletConvolution(const WaveletAnalysis &other);

  /** Destructor. */
  virtual ~GenericWaveletConvolution();

  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out);

protected:
  /** Performs the initialization of the time-scale convolution operation. */
  void _init_convolution();

protected:
  /** The list of convolution filters applied for the convolution with the reproducing kernel. */
  std::vector<GenericConvolution<Scalar> *> _reprodKernel;
};

/// Default template instance for double precision.
typedef GenericWaveletConvolution<double> WaveletConvolution;

}


/* ********************************************************************************************* *
 * Implementation of GenericWaveletConvolution
 * ********************************************************************************************* */
template<class Scalar>
wt::GenericWaveletConvolution<Scalar>::GenericWaveletConvolution(
    const Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXd> &Scales)
  : wt::WaveletAnalysis(wavelet, scales), _reprodKernel()
{
  this->_init_convolution();
}

template <class Scalar>
wt::GenericWaveletConvolution<Scalar>::GenericWaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales)
  : wt::WaveletAnalysis(wavelet, scales, Nscales), _reprodKernel()
{
  this->_init_convolution();
}

template <class Scalar>
wt::GenericWaveletConvolution<Scalar>::GenericWaveletConvolution(const WaveletAnalysis &other)
  : wt::WaveletAnalysis(other), _reprodKernel()
{
  this->_init_convolution();
}

template <class Scalar>
wt::GenericWaveletConvolution<Scalar>::~GenericWaveletConvolution() {
  for (size_t i=0; i<_reprodKernel.size(); i++) {
    delete _reprodKernel[i];
  }
}

template <class Scalar>
void wt::GenericWaveletConvolution<Scalar>::_init_convolution() {
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

template <class Scalar>
template <class iDerived, class oDerived>
void
wt::GenericWaveletConvolution<Scalar>::operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out)
{
}


#endif // __WT_WAVELETCONVOLUTION_HH__
