#ifndef __WT_EXCEPTION_HH__
#define __WT_EXCEPTION_HH__

#include <sstream>
#include <exception>

namespace wt {

/** Base class of all exceptions. */
class Error: public std::exception, public std::stringstream
{
protected:
  /** Hidden default constructor. */
  Error();
  /** Hidden constructor with message. */
  Error(const std::string &msg);

public:
  /** Copy constructor. */
  Error(const Error &other);
  /** Destructor. */
  virtual ~Error() throw();
  /** Returns the error message, implements @c std::exception interface. */
  virtual const char *what() const throw ();
};


/** Exception being thrown on IO errors. */
class IOError: public Error
{
public:
  /** Default/empty constructor. */
  IOError();
  /** Constructor with message. */
  IOError(const std::string &msg);
  /** Copy constructor. */
  IOError(const IOError &other);
  /** Destructor. */
  virtual ~IOError() throw();
};


/** Exception being thrown on index or dimension errors.
 * @see assertShapeNM, assertShapeN
 * @ingroup core */
class DimensionError: public Error
{
public:
  /** Default/empty constructor. */
  DimensionError();
  /** Constructor with message. */
  DimensionError(const std::string &msg);
  /** Copy constructor. */
  DimensionError(const DimensionError &other);
  /** Destructor. */
  virtual ~DimensionError() throw();
};


/** Exception thrown if some constraints on values are not met.
 * @see assertValue */
class ValueError: public Error
{
public:
  /** Default/empty constructor. */
  ValueError();
  /** Constructor with message. */
  ValueError(const std::string &msg);
  /** Copy constructor. */
  ValueError(const ValueError &other);
  /** Destructor. */
  virtual ~ValueError() throw();
};

}

/** Throws a @c DimensionError exception if @c x is not a (N x 1) matrix (vector). */
#define assertShapeN(x, N) \
  if ((int(N) != x.rows()) && (1!=x.cols())) { \
    wt::DimensionError err; \
    err << __FILE__ << "@" << __LINE__ << ": Expected " << #x << " to be of shape (" << N << " x 1), got (" << x.rows()\
        << " x " << x.cols() << ")."; \
    throw err; \
  }

/** Throws a @c DimensionError exception if @c x is not a (N x M) matrix. */
#define assertShapeNM(x, N, M) \
  if ((int(N) != x.rows()) || (int(M) != x.cols())) { \
    wt::DimensionError err; \
    err << __FILE__ << "@" << __LINE__ << ": Expected " << #x << " to be of shape (" << N << " x " << M \
        << "), got (" << x.rows() << " x " << x.cols() << ")."; \
    throw err; \
  }

/** Throws a @c ValueError exception if assert @c x is not met. */
#define assertValue(x) \
  if (! (x)) { \
    wt::ValueError err; \
    err << __FILE__ << "@" << __LINE__ << ": Expected " << #x << "!"; \
    throw err; \
  }

#endif // __WT_EXCEPTION_HH__
