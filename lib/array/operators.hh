#ifndef __WT_OPERATORS_HH__
#define __WT_OPERATORS_HH__

#include "array.hh"

template <class Scalar>
wt::Array<Scalar> operator+(const wt::Array<Scalar> &lhs, const wt::Array<Scalar> &rhs) {
  ASSERT_SAME_SHAPE(lhs, rhs);
  wt::Array<Scalar> res(lhs.rank(), lhs.dims());
  wt::ArrayIterator<Scalar> litem = lhs.begin();
  wt::ArrayIterator<Scalar> ritem = rhs.begin();
  wt::ArrayIterator<Scalar> oitem = res.begin();
  wt::ArrayIterator<Scalar> oend  = res.end();
  for (; oitem != oend; litem++, ritem++, oitem++) {
    (*oend) = *litem + *ritem;
  }
  return res;
}

template <class Scalar>
wt::Array<Scalar> operator-(const wt::Array<Scalar> &lhs, const wt::Array<Scalar> &rhs) {
  ASSERT_SAME_SHAPE(lhs, rhs);
  wt::Array<Scalar> res(lhs.rank(), lhs.dims());
  wt::ArrayIterator<Scalar> litem = lhs.begin();
  wt::ArrayIterator<Scalar> ritem = rhs.begin();
  wt::ArrayIterator<Scalar> oitem = res.begin();
  wt::ArrayIterator<Scalar> oend  = res.end();
  for (; oitem != oend; litem++, ritem++, oitem++) {
    (*oend) = *litem - *ritem;
  }
  return res;
}

template <class Scalar>
wt::Array<Scalar> operator*(const wt::Array<Scalar> &lhs, const wt::Array<Scalar> &rhs) {
  ASSERT_SAME_SHAPE(lhs, rhs);
  wt::Array<Scalar> res(lhs.rank(), lhs.dims());
  wt::ArrayIterator<Scalar> litem = lhs.begin();
  wt::ArrayIterator<Scalar> ritem = rhs.begin();
  wt::ArrayIterator<Scalar> oitem = res.begin();
  wt::ArrayIterator<Scalar> oend  = res.end();
  for (; oitem != oend; litem++, ritem++, oitem++) {
    (*oend) = *litem * *ritem;
  }
  return res;
}

template <class Scalar>
wt::Array<Scalar> operator/(const wt::Array<Scalar> &lhs, const wt::Array<Scalar> &rhs) {
  ASSERT_SAME_SHAPE(lhs, rhs);
  wt::Array<Scalar> res(lhs.rank(), lhs.dims());
  wt::ArrayIterator<Scalar> litem = lhs.begin();
  wt::ArrayIterator<Scalar> ritem = rhs.begin();
  wt::ArrayIterator<Scalar> oitem = res.begin();
  wt::ArrayIterator<Scalar> oend  = res.end();
  for (; oitem != oend; litem++, ritem++, oitem++) {
    (*oend) = *litem / *ritem;
  }
  return res;
}


#endif // __WT_OPERATORS_HH__
