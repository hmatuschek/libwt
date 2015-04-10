#ifndef __WT_WAVELETSYNTHESIS_HH__
#define __WT_WAVELETSYNTHESIS_HH__

#include "types.hh"
#include "wavelet.hh"
#include "convolution.hh"
#include <vector>


namespace wt {

class WaveletSynthesis
{
public:
  /** Constructor. */
  WaveletSynthesis(const Wavelet &wavelet, const RVector &scales);
  /** Constructor using double pointers for the python/numpy interface. */
  WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales);
  /** Destructor. */
  virtual ~WaveletSynthesis();

  /** Returns the number of scales of this wavelet transform. */
  inline size_t nScales() const { return _scales.size(); }
  /** Returns the scales of the wavelet transform. */
  inline const RVector &scales() const { return _scales; }
  /** Returns the (the first @c Nscales) scales of the wavelet transform stored in @c outScales. */
  inline void scales(double *outScales, int Nscales) {
    for (int i=0; i<std::min(int(_scales.size()), Nscales); i++) {
      outScales[i] = _scales(i);
    }
  }
  /** Returns the wavelet instance of this transform. */
  inline const Wavelet &wavelet() const { return _wavelet; }

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
  void init_trafo();

protected:
  /** The (mother-) wavelet to of the transform. */
  Wavelet _wavelet;
  /** The scales (in samples) of the transform. */
  RVector _scales;
  /** The list of convolution filters applied for the wavelet transform. */
  std::vector<Convolution *> _filterBank;
};

}


#endif // __WT_WAVELETSYNTHESIS_HH__
