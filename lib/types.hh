#ifndef __WT_TYPES_HH__
#define __WT_TYPES_HH__

#include <complex>
#include <cmath>
#include <cinttypes>
#include "object.hh"
#include <Eigen/Dense>

namespace wt {

/** Type traits for different scalar types. */
template <typename Scalar>
class Traits
{
public:
  /** The real scalar type. */
  typedef Scalar Real;
  /** The complex scalar type. */
  typedef std::complex<Real> Complex;
  /** The real-valued vector type. */
  typedef Eigen::Matrix<Real, Eigen::Dynamic, 1> RVector;
  /** The complex-valued vector type. */
  typedef Eigen::Matrix<Complex, Eigen::Dynamic, 1> CVector;
  /** The real-valued matrix type. */
  typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> RMatrix;
  /** The complex-valued matrix type. */
  typedef Eigen::Matrix<Complex, Eigen::Dynamic, Eigen::Dynamic> CMatrix;
};

}

/// Implements ceil(a/b) for integers a & b.
#define WT_IDIV_CEIL(a,b) ( (a/b) + ((a%b)?1:0) )

#endif // __WT_TYPES_HH__
