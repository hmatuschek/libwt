#ifndef __WT_WILSON_HH__
#define __WT_WILSON_HH__

#include "types.hh"
#include <vector>

namespace wt {

template <class Matrix>
void wilson(const std::vector<Matrix> &S, std::vector<Matrix> &phi);

template <class Matrix>
void wilson_step(std::vector< Matrix > &S, int niter=5);


/* ********************************************************************************************* *
 * Implementation of Wilson decomposition
 * ********************************************************************************************* */
template <>
void wilson<Matrix>(std::vector< Matrix > &S, int niter=5) {
  // Get inital guess for the factor phi from S. That is,
  //  the cholesky factor of S[0]
  std::vector<Matrix> phi; phi.reserve(S.size());
  Matrix U = S[0].llt().MatrixU();
  for (size_t i=0; i<S.size(); i++) {
    phi.push_back(U);
  }

  // Update phi iteratively (in-place)
  for (int i=0; i<niter; i++) {
    double eps = wilson_step<Matrix>(S, phi);
  }

  // Store phi in S
  for (size_t i=0; i<S.size(); i++) {
    S[i] = phi[i];
  }
}

template <>
void wilson_step<Matrix>(const std::vector<Matrix> &S, std::vector<Matrix> &phi) {

}
}

#endif // WILSON_HH
