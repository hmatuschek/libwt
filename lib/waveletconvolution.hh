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
  typedef typename Traits<Scalar>::Complex Complex;
  typedef typename Traits<Scalar>::RVector RVector;
  typedef typename Traits<Scalar>::CVector CVector;
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

  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out);

protected:
  /** Performs the initialization of the time-scale convolution operation. */
  void _init_convolution();

protected:
  /** The list of convolution filters applied for the convolution with the reproducing kernel. */
  std::vector<GenericConvolution<Scalar> *> _reprodKernel;
  /** The vector of scale differences. */
  RVector _dScale;
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
  _reprodKernel.reserve(_scales.size());
  _dScale.resize(_scales.rows());
  double maxScale = _scales.maxCoeff();

  for (int i=0; i<_scales.rows(); i++) {
    if ((i+1) < _scales.rows())
      _dScale(i) = _scales(i+1)-_scales(i);
    else
      _dScale(i) = _dScale(i-1);
  }

  // For every scale of the input ...
  for (int i=0; i<_scales.size(); i++) {
    // Determine the approx. scale range the rep. kernel is supported on.
    size_t N = FFT<Scalar>::roundUp(std::ceil(maxScale/_scales[i]*4*_wavelet.cutOffTime()));
    CMatrix kernel(N, _scales.size());
    // ...evaluate the kernel at every scale.
    for (int j=0; j<_scales.size(); j++) {
      for (size_t l=0; l<N; l++) {
        kernel(l,j) = (_wavelet.normConstant() *
                       _wavelet.evalRepKern((l-double(N)/2)/_scales[i], _scales[j]/_scales[i]) /
                       (_scales[i]*_scales[i]) * _dScale(i));
      }
    }
    _reprodKernel.push_back(new GenericConvolution<Scalar>(kernel));
  }
  // done.
}

template <class Scalar>
template <class iDerived, class oDerived>
void
wt::GenericWaveletConvolution<Scalar>::operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out)
{
  assertShapeNM(transformed, out.rows(), _scales.rows());
  assertShapeNM(out, transformed.rows(), _scales.rows());

  out.setZero();
  CMatrix tempRes(transformed.rows(), _scales.rows());
  // for every input scale
  for (int i=0; i<_scales.rows(); i++) {
    _reprodKernel[i]->apply(transformed.col(i), tempRes);
    out.derived() += tempRes;
  }
}


#endif // __WT_WAVELETCONVOLUTION_HH__
