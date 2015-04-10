#ifndef __WT_WAVELET_HH__
#define __WT_WAVELET_HH__

#include "types.hh"

namespace wt {


/** Base class of all wavelet objects. */
class WaveletObj: public Object
{
protected:
  /** Hidden constructor. */
  WaveletObj();

public:
  /** Destructor. */
  virtual ~WaveletObj();
  /** Needs to be implemented by any specialization to evaluate the actual wavelet at time @c t. */
  virtual CScalar eval(const Scalar &t) const = 0;
  /** Returns the "width" of the (unscaled) wavelet in time. Needs to be implemented by any
   * wavelet. */
  virtual double timeWidth() const = 0;
  /** Returns the spectral width of the unscaled wavelet in frequency (frequency resolution).
   * This can be considered as the "width" of the Fourier transformed wavelet. */
  virtual double specWidth() const = 0;
};


/** Base class of all wavelet object containers.
 * This container can hold any wavelet object and provides access to the generic methods of these
 * objects. */
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

  /** Evaluates the (unscaled) wavelet at the specified position. */
  inline CScalar eval(const Scalar &t) const {
    return _wavelet->eval(t);
  }
  /** Evaluates the (unscaled) wavelet at the specified position. */
  inline CScalar operator() (const Scalar &t) const {
    return _wavelet->eval(t);
  }

  /** Returns the width of the wavelet in time. */
  inline double timeWidth() const {
    return _wavelet->timeWidth();
  }
  /** Returns the width of the wavelet in frequencies. */
  inline double specWidth() const {
    return _wavelet->specWidth();
  }

protected:
  /** Holds a reference to the wavelet object. */
  WaveletObj *_wavelet;
};


/** Implements the Morlet wavelet. */
class MorletObj: public WaveletObj
{
public:
  MorletObj(double dff=2);
  virtual ~MorletObj();

  virtual CScalar eval(const Scalar &t) const;
  virtual double timeWidth() const;
  virtual double specWidth() const;

protected:
  double _dff;
};

/** Container class for the Morlet wavelet. */
class Morlet: public Wavelet
{
public:
  Morlet(double dff=2.0);
  explicit Morlet(MorletObj *obj);
  Morlet(const Morlet &other);
  virtual ~Morlet();

  Morlet &operator=(const Morlet &other);

protected:
  MorletObj *_morlet;
};


class CauchyObj: public WaveletObj
{
public:
  CauchyObj(double alpha=1);
  virtual ~CauchyObj();

  virtual CScalar eval(const Scalar &t) const;
  virtual double timeWidth() const;
  virtual double specWidth() const;

protected:
  double _alpha;
};

class Cauchy: public Wavelet
{
public:
  Cauchy(double alpha=1.0);
  explicit Cauchy(CauchyObj *obj);
  Cauchy(const Cauchy &other);
  virtual ~Cauchy();

  Cauchy &operator=(const Cauchy &other);

protected:
  CauchyObj *_cauchy;
};

}

#endif // __WT_WAVELET_HH__
