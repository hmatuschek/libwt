#ifndef __WT_TYPES_HH__
#define __WT_TYPES_HH__

#include <complex>
#include <cmath>
#include "object.hh"
#include <Eigen/Eigen>

namespace wt {

/*
typedef double Scalar;
typedef std::complex<Scalar> CScalar;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> RVector;
typedef Eigen::Matrix<CScalar, Eigen::Dynamic, 1> CVector;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> RMatrix;
typedef Eigen::Matrix<CScalar, Eigen::Dynamic, Eigen::Dynamic> CMatrix;
*/

template <typename Scalar>
class Traits
{
public:
  typedef Scalar Real;
  typedef std::complex<Real> Complex;
  typedef Eigen::Matrix<Real, Eigen::Dynamic, 1> RVector;
  typedef Eigen::Matrix<Complex, Eigen::Dynamic, 1> CVector;
  typedef Eigen::Matrix<Real, Eigen::Dynamic, Eigen::Dynamic> RMatrix;
  typedef Eigen::Matrix<Complex, Eigen::Dynamic, Eigen::Dynamic> CMatrix;
};

}

/// Implements ceil(a/b) for integers a & b.
#define WT_IDIV_CEIL(a,b) ( (a/b) + ((a%b)?1:0) )

#endif // __WT_TYPES_HH__
