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
  // Get inital guess for the factor phi from S
  std::vector<Matrix> phi; phi.reserve(S.size());
  for (size_t i=0; i<S.size(); i++) {
    phi.push_back(S[i].LLT());
  }
  // Update phi iteratively
  for (int i=0; i<niter; i++) {
    wilson_step<Matrix>(S, phi);
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
