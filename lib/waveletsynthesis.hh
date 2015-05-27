#ifndef __WT_WAVELETSYNTHESIS_HH__
#define __WT_WAVELETSYNTHESIS_HH__

#include "waveletanalysis.hh"
#include "convolution.hh"
#include <vector>


namespace wt {

/** Implements the wavelet synthesis, means the reconstruction of the signal from
 * a wavelet transformed.
 * @ingroup analyses */
class WaveletSynthesis: public WaveletAnalysis
{
public:
  /** Constructor. */
  WaveletSynthesis(const Wavelet &wavelet, const RVector &scales);
  /** Constructor using double pointers for the python/numpy interface. */
  WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales);
  /** Constructor from other wavelet analysis. */
  WaveletSynthesis(const WaveletAnalysis &other);
  /** Destructor. */
  virtual ~WaveletSynthesis();

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
  void init_synthesis();

protected:
  /** The list of convolution filters applied for the wavelet synthesis. */
  std::vector<Convolution *> _filterBank;
};

}


#endif // __WT_WAVELETSYNTHESIS_HH__
