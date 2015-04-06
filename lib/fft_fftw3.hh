#ifndef __WT_FFT_FFTW3_HH__
#define __WT_FFT_FFTW3_HH__

#include <fftw3.h>
#include "types.hh"

namespace wt {

/** Implements the generic FFT-plan interface for the FFTW3 library. */
class FFT
{
public:
  typedef enum {
    FORWARD,
    BACKWARD
  } Direction;

public:
  /** Computes the FFT of the input vector @c in and stores the result into @c out. */
  FFT(CVector &in, CVector &out, Direction dir);
  /** Computes the FFT of the column-vectors @c in and stores the result into the columns of
   * @c out. */
  FFT(CMatrix &in, CMatrix &out, Direction dir);
  FFT(CVector &inout, Direction dir);
  FFT(CMatrix &inout, Direction dir);

  virtual ~FFT();

  void exec();

public:
  static void exec(CVector &in, CVector &out, Direction dir);
  static void exec(CMatrix &in, CMatrix &out, Direction dir);
  static void exec(CVector &inout, Direction dir);
  static void exec(CMatrix &inout, Direction dir);

  /** Computes the smallest samples size larger than or equal to @c N for which the FFT can be
   * computed fast. */
  static size_t roundUp(size_t N);

protected:
  fftw_plan _plan;
};

}

#endif // __WT_FFT_FFTW3_HH__
