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
class WaveletSynthesis: public WaveletAnalysis
{
public:
  typedef typename Traits<Scalar>::Complex Complex;
  typedef typename Traits<Scalar>::RVector RVector;
  typedef typename Traits<Scalar>::CVector CVector;
  typedef typename Traits<Scalar>::CMatrix CMatrix;

public:
  /** Constructor. */
  WaveletSynthesis(const Wavelet &wavelet, const RVector &scales)
    : WaveletAnalysis(wavelet, scales), _filterBank()
  {
    init_synthesis();
  }

  /** Constructor using double pointers for the python/numpy interface. */
  WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales)
    : WaveletAnalysis(wavelet, scales, Nscales), _filterBank()
  {
    init_synthesis();
  }

  /** Constructor from other wavelet analysis. */
  WaveletSynthesis(const WaveletAnalysis &other)
    : WaveletAnalysis(other), _filterBank()
  {
    init_synthesis();
  }

  /** Destructor. */
  virtual ~WaveletSynthesis() {
    typename std::vector<Convolution<Scalar> *>::iterator filter = _filterBank.begin();
    for (; filter != _filterBank.end(); filter++) { delete *filter; }
  }


  /** Performs the wavelet synthesis. */
  template <class iDerived, class oDerived>
  void operator() (const Eigen::DenseBase<iDerived> &transformed, Eigen::DenseBase<oDerived> &out)
  {
    CVector last(transformed.rows()), current(transformed.rows());
    // Clear output vector
    out.setZero();
    // If there is no filter bank -> done.
    if (0 == this->_filterBank.size()) { return; }
    // Apply first scale
    this->_filterBank[0]->apply(transformed.col(0), last);
    // Iterate over all scales and integrate over scales (mid-point method)
    for (size_t j=1; j<this->_filterBank.size(); j++) {
      // Perform FFT convolution
      this->_filterBank[j]->apply(transformed.col(j), current);
      out.head(transformed.rows()) += ((this->_scales[j]-this->_scales[j-1])/2)*(current+last);
      // store current into last
      last.swap(current);
    }
  }

protected:
  /** Initializes the filter bank for the synthesis operation. */
  void init_synthesis() {
    // Sort scales (ascending order)
    std::sort(_scales.derived().data(), _scales.derived().data()+_scales.size());

    _filterBank.clear();
    _filterBank.reserve(_scales.size());
    // Determine kernel size for every scale and round up to next integer for which the FFT can
    // be computed fast. Also group the resulting kernel lengths
    for (int j=0; j<_scales.size(); j++) {
      size_t N = FFT<Scalar>::roundUp(std::ceil(_scales[j]*2*_wavelet.cutOffTime()));
      CVector kernel(N);
      for (size_t i=0; i<N; i++) {
        kernel(i) = (_wavelet.evalSynthesis((i-double(N)/2)/_scales[j])/(_scales[j]*_scales[j]));
      }
      _filterBank.push_back(new Convolution<Scalar>(kernel));
    }
  }

protected:
  /** The list of convolution filters applied for the wavelet synthesis. */
  std::vector<Convolution<Scalar> *> _filterBank;
};

}


#endif // __WT_WAVELETSYNTHESIS_HH__
