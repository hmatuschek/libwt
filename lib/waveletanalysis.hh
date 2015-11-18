/** @defgroup analyses Wavelet analyses
 * This group documents all implemented wavelet analses.
 */

#ifndef __WT_WAVELETANALYSIS_HH__
#define __WT_WAVELETANALYSIS_HH__

#include "types.hh"
#include "api.hh"

namespace wt {

/** Abstract base class of all wavelet analyses (transform and synthesis).
 * It simply holds the wavelet and the scales of the analysis.
 * @ingroup analyses */
class WaveletAnalysis
{
protected:
  /** Hidden constructor from the specified wavelet and scales. */
  WaveletAnalysis(const Wavelet &wavelet, const RVector &scales);
  /** Hidden constructor from the specified wavelet and scales. */
  WaveletAnalysis(const Wavelet &wavelet, double *scales, int Nscales);

public:
  /** Copy constructor. */
  WaveletAnalysis(const WaveletAnalysis &other);
  /** Destructor. */
  virtual ~WaveletAnalysis();

  /** Returns the number of scales of this wavelet transform. */
  inline size_t nScales() const { return _scales.size(); }
  /** Returns the scales of the wavelet transform. */
  inline const RVector &scales() const { return _scales; }
  /** Returns the (the first @c Nscales) scales of the wavelet transform stored in @c outScales. */
  inline void scales(double *outScales, int Nscales) const {
    for (int i=0; i<std::min(int(_scales.size()), Nscales); i++) {
      outScales[i] = _scales(i);
    }
  }
  /** Returns the wavelet instance of this transform. */
  inline const Wavelet &wavelet() const { return _wavelet; }

protected:
  /** The (mother-) wavelet to of the transform. */
  Wavelet _wavelet;
  /** The scales (in samples) of the transform. */
  RVector _scales;
};

}

#endif // __WT_WAVELETANALYSIS_HH__
