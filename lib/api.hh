/** @defgroup api Interface classes
 *
 * This module collects all container classes providing a simplified interface to libwt. These container classes implement
 * the reference counting which frees unused objects automatically. This eases the usage and the interface to foreigen
 * languages as the user is not responsible for the deletion of the instances.
 */
#ifndef __WT_API_HH__
#define __WT_API_HH__

#include "types.hh"
#include "wavelet.hh"

namespace wt {

/** Interface for a progress delegate. You may also use the @c ProgressDelegate template class
 * to pass callback methods as delegates.
 * @ingroup api */
class ProgressDelegateInterface
{
public:
  /** Needs to be implemented by all progress delegates. */
  virtual void operator() (double frac)  = 0;
};


/** Implements a generic progress delegate that calls a method of some object passed to the
 * constructor.
 * @ingroup api */
template <class T>
class ProgressDelegate: public ProgressDelegateInterface
{
public:
  /** Constructs a progress delegate for the given @c instance calling its specified @c method. */
  ProgressDelegate(T &instance, void (T::*method)(double))
    : ProgressDelegateInterface(), _instance(instance), _method(method) { }
  virtual ~ProgressDelegate() {  }

  /** Implements the @c ProgressDelegateInterface. */
  virtual void operator() (double frac) {
    (this->_instance.*this->_method)(frac);
  }

protected:
  /** A weak reference to the instance. */
  T &_instance;
  /** A reference to the method being called on progress. */
  void (T::*_method)(double);
};


/** Base class of all wavelet object containers.
 * This container can hold any wavelet object and provides access to the generic methods of these
 * objects.
 * @ingroup api */
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
  inline std::complex<double> evalAnalysis(const double &t) const {
    return _wavelet->evalAnalysis(t);
  }

  /** Evaluates the (unscaled) analysis wavelet at the specified position. */
  inline std::complex<double> evalSynthesis(const double &t) const {
    return _wavelet->evalSynthesis(t);
  }

  /** Revaluates the reproducing kernel located at \f$(0,1)\f$ at the given time \f$b\f$ and
   * scale \f$a\f$. */
  inline std::complex<double> evalRepKern(const double &b, const double &a) const {
    return _wavelet->evalRepKern(b, a);
  }

  /** Returns the normalization constant for the wavelet synthesis (inverse transform). */
  inline double normConstant() const {
    return _wavelet->normConstant();
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


/** The Cauchy or Paul wavelet.
 * In contrast to the @c Morlet wavelet, the Cauchy or Paul wavelet is a proper wavelet with a
 * similar good localization in time and scale.
 *
 * There are different definitoins of the Cauchy wavelet around. Here one is implemented where
 * the center frequency is always 1 irrespective of the value of \f$\alpha\f$:
 * \f[
 * g(t) = h(t) = \left(1-i\,\frac{2\pi\,t}{\alpha}\right)^{-(1+\alpha)}\,,
 * \f]
 * and its reproducing kernel
 * \f[
 * P_{g,h}(b, a) = \Gamma(2\alpha+1)\,a^{\alpha+1}\,\left(1+a-\frac{i\,b}{a}\right)^{-(2\alpha+1)}\,.
 * \f]
 * @ingroup api */
class Cauchy: public Wavelet
{
public:
  /** Object type of the container. */
  typedef CauchyObj ObjectType;

public:
  /** Constructor.
   * @param alpha Specifies the time resolution. */
  explicit Cauchy(double alpha=2.0);
  /** Packs the given @c CauchyObj instance into a container. The ownership is taken. */
  Cauchy(CauchyObj *obj);
  /** Copy constructor, manages references. */
  Cauchy(const Cauchy &other);
  /** Destructor. */
  virtual ~Cauchy();
  /** Assignment operator, manages references. */
  Cauchy &operator=(const Cauchy &other);

  /** Returns the frequency resolution parameter. */
  double alpha() const;

protected:
  /** Holds a reference to the @c CauchyObj instance. */
  CauchyObj *_cauchy;
};


/** The Morlet wavelet.
 *
 * \f[
 *  g(t) = \sqrt{\frac{\delta}{2\pi}}\exp(2\pi\,i\,t-t^2\,\delta)\,,
 * \f]
 * where \f$\delta\f$ specifies the time-frequency resolution of the wavelet. Default
 * \f$\delta=2\f$.
 * @ingroup api */
class Morlet: public Wavelet
{
public:
  /** Object type of the container. */
  typedef MorletObj ObjectType;

public:
  /** Constructor.
   * @param dff Specifies the frequency resolution. */
  Morlet(double dff=2.0);
  /** Packs the given @c MorletObj instance into a container, the constructor takes the
   * ownership of the instance. */
  Morlet(MorletObj *obj);
  /** Copy constructor, manages references. */
  Morlet(const Morlet &other);
  /** Destructor. */
  virtual ~Morlet();
  /** Assignment operator, manages references. */
  Morlet &operator=(const Morlet &other);

  /** Returns the frequency resolution parameter. */
  double dff() const;

protected:
  /** Holds a reference to the actual instance @c MorletObj instance. */
  MorletObj *_morlet;
};

/** Samples a regular grid in the range \f$[a,b)\f$ into @c out.
 * @ingroup api */
void linear_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out);
/** Samples a dyadic grid in the range \f$[a,b)\f$ into @c out.
 * @ingroup api */
void dyadic_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out);
/** Samples a decadic grid in the range \f$[a,b)\f$ into @c out.
 * @ingroup api */
void decadic_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out);

} // namespace wt

#endif // API_H
