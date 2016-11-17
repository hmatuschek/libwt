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
  /** Complex scalar type. */
  typedef typename Traits<Scalar>::Complex Complex;
  /** Real vector type. */
  typedef typename Traits<Scalar>::RVector RVector;
  /** Complex vector type. */
  typedef typename Traits<Scalar>::CVector CVector;
  /** Complex matrix type. */
  typedef typename Traits<Scalar>::CMatrix CMatrix;

public:
  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  GenericWaveletConvolution(const Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXd> &scales);

  /** Constructs the convolution with the reproducting kernel of the specified wavelet pair. */
  GenericWaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales);

  /** Copy constructor. */
  GenericWaveletConvolution(const WaveletAnalysis &other);

  /** Destructor. */
  virtual ~GenericWaveletConvolution();

  /** Performs the convolution of the @c transformed with the reproducing kernel and stores
   * the result into @c out. The optional @c progress allows to pass an instance implementing
   * the @c ProgressDelegateInterface. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out,
                   ProgressDelegateInterface *progress=0);

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
    const Wavelet &wavelet, const Eigen::Ref<const Eigen::VectorXd> &scales)
  : WaveletAnalysis(wavelet, scales), _reprodKernel()
{
  this->_init_convolution();
}

template <class Scalar>
wt::GenericWaveletConvolution<Scalar>::GenericWaveletConvolution(const Wavelet &wavelet, double *scales, size_t Nscales)
  : WaveletAnalysis(wavelet, scales, Nscales), _reprodKernel()
{
  this->_init_convolution();
}

template <class Scalar>
wt::GenericWaveletConvolution<Scalar>::GenericWaveletConvolution(const WaveletAnalysis &other)
  : WaveletAnalysis(other), _reprodKernel()
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
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  logDebug() << "Construct wavelet projection on " << _scales.size() << " scales in ["
             << _scales(0) << "," << _scales(_scales.size()-1) << "].";

  _reprodKernel.reserve(_scales.size());

  // For every scale of the input ...
  for (int i=0; i<_scales.size(); i++) {
    // Determine the approx. time-scale range, the rep. kernel is supported on.
    size_t N = FFT<Scalar>::roundUp(std::ceil(_scales[i]*2*_wavelet.cutOffTime()));
    CMatrix kernel(N, _scales.size());

    // ...evaluate the kernel at every scale.
    for (int j=0; j<_scales.size(); j++) {
      for (size_t l=0; l<N; l++) {
        kernel(l,j) = _wavelet.normConstant() *
            _wavelet.evalRepKern((l-double(N)/2)/_scales[i], _scales[j]/_scales[i])
            / _scales[i] / _scales[i];
      }
    }
    _reprodKernel.push_back(new GenericConvolution<Scalar>(kernel));
  }
  // done.
}

template <class Scalar>
template <class iDerived, class oDerived>
void
wt::GenericWaveletConvolution<Scalar>::operator() (
    const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out,
    ProgressDelegateInterface *progress)
{
  assertShapeNM(transformed, out.rows(), this->_scales.rows());
  assertShapeNM(out, transformed.rows(), this->_scales.rows());

  CMatrix tempRes1(transformed.rows(), this->_scales.rows());
  CMatrix tempRes2(transformed.rows(), this->_scales.rows());
  out.setZero();

  // for every input scale
  this->_reprodKernel[0]->apply(transformed.col(0), tempRes1);
  for (int i=1; i<this->_scales.size(); i++) {
    if (i & 1) // odd
      this->_reprodKernel[i]->apply(transformed.col(i), tempRes2);
    else // even
      this->_reprodKernel[i]->apply(transformed.col(i), tempRes1);
    out.derived() += ( (this->_scales(i)-this->_scales(i-1))/2 * (tempRes1+tempRes2) );
    if (progress)
      (*progress)(double(i+1)/this->scales().size());
  }
}


#endif // __WT_WAVELETCONVOLUTION_HH__
