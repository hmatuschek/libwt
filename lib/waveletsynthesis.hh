#ifndef __WT_WAVELETSYNTHESIS_HH__
#define __WT_WAVELETSYNTHESIS_HH__

#include "waveletanalysis.hh"
#include "convolution.hh"
#include <vector>


namespace wt {

/** Implements the wavelet synthesis, means the reconstruction of the signal from
 * a wavelet transformed.
 * @ingroup analyses */
template <class Scalar>
class GenericWaveletSynthesis: public WaveletAnalysis
{
public:
  /// Complex scalar type.
  typedef typename Traits<Scalar>::Complex Complex;
  /// Real vector type.
  typedef typename Traits<Scalar>::RVector RVector;
  /// Complex vector type.
  typedef typename Traits<Scalar>::CVector CVector;
  /// Complex matrix type.
  typedef typename Traits<Scalar>::CMatrix CMatrix;

public:
  /** Constructor. */
  GenericWaveletSynthesis(const Wavelet &wavelet, const RVector &scales);

  /** Constructor using double pointers for the python/numpy interface. */
  GenericWaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales);

  /** Constructor from other wavelet analysis. */
  GenericWaveletSynthesis(const WaveletAnalysis &other);

  /** Destructor. */
  virtual ~GenericWaveletSynthesis();

  /** Performs the wavelet synthesis. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out,
                   ProgressDelegateInterface *progress=0);

protected:
  /** Initializes the filter bank for the synthesis operation. */
  void init_synthesis();

protected:
  /** The list of convolution filters applied for the wavelet synthesis. */
  std::vector<GenericConvolution<Scalar> *> _filterBank;
};

typedef GenericWaveletSynthesis<double> WaveletSynthesis;

}



/* ********************************************************************************************* *
 * Implementation of GenericWaveletSynthesis
 * ********************************************************************************************* */
template <class Scalar>
wt::GenericWaveletSynthesis<Scalar>::GenericWaveletSynthesis(const Wavelet &wavelet, const RVector &scales)
  : WaveletAnalysis(wavelet, scales), _filterBank()
{
  this->init_synthesis();
}

template <class Scalar>
wt::GenericWaveletSynthesis<Scalar>::GenericWaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales)
  : WaveletAnalysis(wavelet, scales, Nscales), _filterBank()
{
  this->init_synthesis();
}

template <class Scalar>
wt::GenericWaveletSynthesis<Scalar>::GenericWaveletSynthesis(const WaveletAnalysis &other)
  : WaveletAnalysis(other), _filterBank()
{
  this->init_synthesis();
}

template <class Scalar>
wt::GenericWaveletSynthesis<Scalar>::~GenericWaveletSynthesis() {
  typename std::vector<GenericConvolution<Scalar> *>::iterator filter = _filterBank.begin();
  for (; filter != _filterBank.end(); filter++) { delete *filter; }
}

template <class Scalar>
void
wt::GenericWaveletSynthesis<Scalar>::init_synthesis() {
  // Sort scales (ascending order)
  std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

  logDebug() << "Construct wavelet synthesis over " << _scales.size() << " scales in ["
             << _scales(0) << "," << _scales(_scales.size()-1) << "].";

  Eigen::VectorXd dScale(_scales.size());
  for (size_t i=1; i<_scales.size(); i++) {
    dScale(i) = _scales(i)-_scales(i-1);
  }
  dScale(0) = dScale(1);

  _filterBank.clear();
  _filterBank.reserve(_scales.size());
  // Determine kernel size for every scale and round up to next integer for which the FFT can
  // be computed fast. Also group the resulting kernel lengths
  for (int j=0; j<_scales.size(); j++) {
    size_t N = FFT<Scalar>::roundUp(std::ceil(_scales[j]*2*_wavelet.cutOffTime()));
    CVector kernel(N);
    for (size_t i=0; i<N; i++) {
      kernel(i) = _wavelet.normConstant() *
          _wavelet.evalSynthesis((i-double(N)/2)/_scales[j]) /
          _scales[j]/_scales[j];
    }
    _filterBank.push_back(new GenericConvolution<Scalar>(kernel));
  }
}

template <class Scalar>
template <class iDerived, class oDerived>
void
wt::GenericWaveletSynthesis<Scalar>::operator() (
    const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out,
    ProgressDelegateInterface *progress)
{
  CVector last(transformed.rows()), current(transformed.rows());
  // Clear output vector
  out.setZero();

  // If there is no filter bank -> done.
  if (0 == this->_filterBank.size())
    return;

  // Apply first scale
  this->_filterBank[0]->apply(transformed.col(0), last);
  // Iterate over all scales and integrate over scales (mid-point method)
  for (size_t j=1; j<this->_filterBank.size(); j++) {
    // Perform FFT convolution
    this->_filterBank[j]->apply(transformed.col(j), current);
    out.head(transformed.rows()) += ((this->_scales[j]-this->_scales[j-1])/2)*(current+last);
    // store current into last
    last.swap(current);
    if (progress)
      (*progress)(double(j)/this->_filterBank.size());
  }
}



#endif // __WT_WAVELETSYNTHESIS_HH__
