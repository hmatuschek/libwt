#ifndef __WT_WAVELET_HH__
#define __WT_WAVELET_HH__

#include "types.hh"

namespace wt {

/** Base class of all wavelet (-pair) objects. */
class WaveletObj: public Object
{
protected:
  /** Hidden constructor. */
  WaveletObj();

public:
  /** Destructor. */
  virtual ~WaveletObj();
  /** Needs to be implemented by any specialization to evaluate the actual analysis wavelet at
   * time @c t. */
  virtual std::complex<double> evalAnalysis(const double &t) const = 0;
  /** Needs to be implemented by any specialization to evaluate the actual synthesis wavelet at
   * time @c t. */
  virtual std::complex<double> evalSynthesis(const double &t) const = 0;
  /** Needs to be implemented by any specialization to evaluate the reproducing kernel of the wavelet
   * pair at the specified scale and time. Evaluates the reproducing kernel located at time 0 and
   * scale 1 at the given time and scale. */
  virtual std::complex<double> evalRepKern(const double &b, const double &a) const = 0;
  /** Returns the normalization constant, \f$c_{gh}\f$. */
  virtual double normConstant() const;
  /** Returns the "width" of the (unscaled) wavelet in time. Needs to be implemented by any
   * wavelet. */
  virtual double cutOffTime() const = 0;
  /** Returns the spectral width of the unscaled wavelet in frequency (frequency resolution).
   * This can be considered as the "width" of the Fourier transformed wavelet. */
  virtual double cutOffFreq() const = 0;
};


/** Implements the Morlet wavelet.
 * Do not use this class directly, consider using the associated container @c Morlet. */
class MorletObj: public WaveletObj
{
public:
  /** Constructor.
   * @param dff Specifies the frequency resolution. */
  explicit MorletObj(double dff=2);
  /** Destructor. */
  virtual ~MorletObj();

  /** Returns the frequency resolution parameter. */
  double dff() const;

  /** Evaluates the mother wavelet at the specified time. */
  virtual std::complex<double> evalAnalysis(const double &t) const;
  /** Evaluates the mother wavelet at the specified time. */
  virtual std::complex<double> evalSynthesis(const double &t) const;
  /** Evaluates the reproducing kernel located at time 0 and scale 1 at the given time and scale. */
  virtual std::complex<double> evalRepKern(const double &b, const double &a) const;

  /** Returns the with of the mother wavelet in the time domain. */
  virtual double cutOffTime() const;
  /** Returns the with of the mother wavelet in the frequency domain. */
  virtual double cutOffFreq() const;

protected:
  /** Holds the frequency resolution parameter. */
  double _dff;
};


/** Implementation of an augmented Cauchy or Paul wavelet.
 * Do not use this class directly, consider using the associated container @c Cauchy. */
class CauchyObj: public WaveletObj
{
public:
  /** Constructor.
   * @param alpha Specifies the time resolution. */
  explicit CauchyObj(double alpha=2);
  /** Destructor. */
  virtual ~CauchyObj();

  /** Returns the frequency resolution parameter. */
  double alpha() const;

  /** Evaluates the mother wavelet at the given time. */
  virtual std::complex<double> evalAnalysis(const double &t) const;
  /** Evaluates the mother wavelet at the given time. */
  virtual std::complex<double> evalSynthesis(const double &t) const;
  /** Evaluates the reproducing kernel located at time 0 and scale 1 at the given time and scale. */
  virtual std::complex<double> evalRepKern(const double &b, const double &a) const;
  /** Returns the normalization constant. */
  virtual double normConstant() const;

  /** Returns the width of the mother wavelet in the time domain. */
  virtual double cutOffTime() const;
  /** Returns the width of the mother wavelet in the frequency domain. */
  virtual double cutOffFreq() const;

protected:
  /** Holds the order. */
  double _alpha;
  /** Holds the normalization constant, \f$c_{gh}\f$. */
  double _norm;
};

}

#endif // __WT_WAVELET_HH__
