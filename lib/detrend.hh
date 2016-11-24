#ifndef __WT_DETREND_HH__
#define __WT_DETREND_HH__

#include "types.hh"
#include <iostream>


namespace wt {

/** Removes the mean and slope from the data, hence detrending a time series and removing its
 * offset. */
template <class oDerived>
void detrend(Eigen::DenseBase<oDerived> &x) {
  typedef typename oDerived::Scalar Scalar;

  Scalar n = x.size();
  Scalar xsum = (n*(n-Scalar(1)))/Scalar(2);
  Scalar xx = (n*(n-Scalar(1))*(Scalar(2)*n-Scalar(1)))/Scalar(6);
  Scalar ysum = 0;
  Scalar xy = 0;

  for (int i=0; i<x.size(); i++) {
    xy   += Scalar(i)*x(i);
    ysum += x(i);
  }

  Scalar a = (n*xy - xsum*ysum)/(n*xx - xsum*xsum);
  Scalar b = (ysum - a*xsum)/n;
  for (int i=0; i<x.size(); i++)
    x(i) -= (b + a*Scalar(i));
}

}
#endif // __WT_DETREND_HH__
