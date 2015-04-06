#ifndef __WT_TYPES_HH__
#define __WT_TYPES_HH__

#include <complex>
#include <cmath>
#include "object.hh"
#include <Eigen/Eigen>

namespace wt {

typedef double Scalar;
typedef std::complex<Scalar> CScalar;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, 1> RVector;
typedef Eigen::Matrix<CScalar, Eigen::Dynamic, 1> CVector;
typedef Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> RMatrix;
typedef Eigen::Matrix<CScalar, Eigen::Dynamic, Eigen::Dynamic> CMatrix;

}

#endif // __WT_TYPES_HH__
