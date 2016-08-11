#ifndef __WT_FFT_FFTW3_HH__
#define __WT_FFT_FFTW3_HH__

#include <fftw3.h>
#include "types.hh"
#include "exception.hh"


namespace wt {

template <class Scalar> class FFT;


/** Implements the generic double precision FFT-plan interface for the FFTW3 library. */
template <>
class FFT<double>
{
public:
  /// The complex vector type.
  typedef typename Traits<double>::CVector CVector;
  /// The complex matrix type.
  typedef typename Traits<double>::CMatrix CMatrix;

public:
  /** Specifies the possible FFT directions. */
  typedef enum {
    FORWARD,  ///< Forward transform.
    BACKWARD  ///< Backward transform.
  } Direction;

public:
  /** Constructs the FFT of the input vector @c in and stores the result into @c out. */
  FFT(CVector &in, CVector &out, Direction dir);
  /** Constructs the FFT of the column vectors @c in and stores the result into the columns of
   * @c out. */
  FFT(CMatrix &in, CMatrix &out, Direction dir);
  /** Construts the in-place FFT of the vector @c inout. */
  FFT(CVector &inout, Direction dir);
  /** Construts the in-place FFT of the column vectors @c inout. */
  FFT(CMatrix &inout, Direction dir);

  /** Destructor. */
  virtual ~FFT();

  /** Executes the FFT. */
  void exec();

public:
  /** Executes the FFT of the input vector @c in and stores the result into @c out. */
  static void exec(CVector &in, CVector &out, Direction dir);
  /** Executes the FFT of the input column vectors @c in and stores the result into the columns of
   * @c out. */
  static void exec(CMatrix &in, CMatrix &out, Direction dir);
  /** Executes the in-place FFT of the vector @c inout. */
  static void exec(CVector &inout, Direction dir);
  /** Executes the in-place FFT of the column vectors @c inout. */
  static void exec(CMatrix &inout, Direction dir);

  /** Computes the smallest samples size larger than or equal to @c N for which the FFT can be
   * computed fast. */
  static size_t roundUp(size_t N);

protected:
  /** The actual FFTW3 plan being executed. */
  fftw_plan _plan;
};

}

#endif // __WT_FFT_FFTW3_HH__
