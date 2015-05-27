/** @defgroup wavelets Wavelets
 * This group lists all defined wavelets. */

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
  virtual CScalar evalAnalysis(const Scalar &t) const = 0;
  /** Needs to be implemented by any specialization to evaluate the actual synthesis wavelet at
   * time @c t. */
  virtual CScalar evalSynthesis(const Scalar &t) const = 0;
  /** Returns the "width" of the (unscaled) wavelet in time. Needs to be implemented by any
   * wavelet. */
  virtual double cutOffTime() const = 0;
  /** Returns the spectral width of the unscaled wavelet in frequency (frequency resolution).
   * This can be considered as the "width" of the Fourier transformed wavelet. */
  virtual double cutOffFreq() const = 0;
};


/** Base class of all wavelet object containers.
 * This container can hold any wavelet object and provides access to the generic methods of these
 * objects.
 * @ingroup wavelets */
class Wavelet : public Container
{
public:
  /** The object type of the container. */
  typedef WaveletObj ObjectType;

public:
  /** Emtpy constructor. */
  Wavelet();
  /** Constructs a wavelet (container) with the specified wavelet object. */
  Wavelet(WaveletObj *obj);
  /** Copy constructor (reference counting). */
  Wavelet(const Wavelet &other);
  /** Destructor, will distroy the containing wavelet object if this was the last reference to
   * it. */
  virtual ~Wavelet();

  /** Assignment operator (reference counting). */
  Wavelet &operator=(const Wavelet &other);

  /** Evaluates the (unscaled) analysis wavelet at the specified position. */
  inline CScalar evalAnalysis(const Scalar &t) const {
    return _wavelet->evalAnalysis(t);
  }

  /** Evaluates the (unscaled) analysis wavelet at the specified position. */
  inline CScalar evalSynthesis(const Scalar &t) const {
    return _wavelet->evalSynthesis(t);
  }

  /** Returns the width of the wavelet in time. */
  inline double cutOffTime() const {
    return _wavelet->cutOffTime();
  }

  /** Returns the width of the wavelet in frequencies. */
  inline double cutOffFreq() const {
    return _wavelet->cutOffFreq();
  }

protected:
  /** Holds a reference to the wavelet object. */
  WaveletObj *_wavelet;
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
  /** Evaluates the mother wavelet at the specified time. */
  virtual CScalar evalAnalysis(const Scalar &t) const;
  /** Evaluates the mother wavelet at the specified time. */
  virtual CScalar evalSynthesis(const Scalar &t) const;
  /** Returns the with of the mother wavelet in the time domain. */
  virtual double cutOffTime() const;
  /** Returns the with of the mother wavelet in the frequency domain. */
  virtual double cutOffFreq() const;

protected:
  /** Holds the frequency resolution parameter. */
  double _dff;
};


/** The Morlet wavelet.
 *
 * \f[
 *  g(t) = \sqrt{\frac{\delta}{2\pi}}\exp(2\pi\,i\,t-t^2\,\delta)\,,
 * \f]
 * where \f$\delta\f$ specifies the time-frequency resolution of the wavelet. Default
 * \f$\delta=2\f$.
 * @ingroup wavelets */
class Morlet: public Wavelet
{
public:
  /** Constructor.
   * @param dff Specifies the frequency resolution. */
  Morlet(double dff=2.0);
  /** Packs the given @c MorletObj instance into a container, the constructor takes the
   * ownership of the instance. */
  explicit Morlet(MorletObj *obj);
  /** Copy constructor, manages references. */
  Morlet(const Morlet &other);
  /** Destructor. */
  virtual ~Morlet();
  /** Assignment operator, manages references. */
  Morlet &operator=(const Morlet &other);

protected:
  /** Holds a reference to the actual instance @c MorletObj instance. */
  MorletObj *_morlet;
};


/** Implementation of the Cauchy wavelet.
 * Do not use this class directly, consider using the associated container @c Cauchy. */
class CauchyObj: public WaveletObj
{
public:
  /** Constructor.
   * @param alpha Specifies the time resolution. */
  explicit CauchyObj(double alpha=1);
  /** Destructor. */
  virtual ~CauchyObj();
  /** Evaluates the mother wavelet at the given time. */
  virtual CScalar evalAnalysis(const Scalar &t) const;
  /** Evaluates the mother wavelet at the given time. */
  virtual CScalar evalSynthesis(const Scalar &t) const;
  /** Returns the width of the mother wavelet in the time domain. */
  virtual double cutOffTime() const;
  /** Returns the width of the mother wavelet in the frequency domain. */
  virtual double cutOffFreq() const;

protected:
  /** Holds the order. */
  double _alpha;
  /** Holds the normalization constant. */
  double _norm;
};

/** The Cauchy or Paul wavelet.
 * @ingroup wavelets */
class Cauchy: public Wavelet
{
public:
  /** Constructor.
   * @param alpha Specifies the time resolution. */
  explicit Cauchy(double alpha=1.0);
  /** Packs the given @c CauchyObj instance into a container. The ownership is taken. */
  Cauchy(CauchyObj *obj);
  /** Copy constructor, manages references. */
  Cauchy(const Cauchy &other);
  /** Destructor. */
  virtual ~Cauchy();
  /** Assignment operator, manages references. */
  Cauchy &operator=(const Cauchy &other);

protected:
  /** Holds a reference to the @c CauchyObj instance. */
  CauchyObj *_cauchy;
};

}

#endif // __WT_WAVELET_HH__
