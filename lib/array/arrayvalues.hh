#ifndef __WT_ARRAYVALUES_HH__
#define __WT_ARRAYVALUES_HH__

#include "arrayiterator.hh"

namespace wt {

template <class T> class Array;

template <class T>
class ArrayValues : public Container
{
public:
  typedef T Scalar;
  typedef ArrayObj<Scalar> ObjectType;

public:
  ArrayValues(ArrayObj<Scalar> *obj): Container(obj), _array(obj) { }
  ArrayValues(const ArrayValues<Scalar> &other) : Container(other), _array(other._array) { }
  virtual ~ArrayValues() { }

  ArrayValues<Scalar> &operator= (const Scalar &value) {
    ArrayIterator<Scalar> item(this->_array), end(this->_array); end.moveToEnd();
    for (; item != end; item++) {
      (*item) = value;
    }
    return *this;
  }

  ArrayValues<Scalar> &operator= (const Array<Scalar> &other) {
    ArrayIterator<Scalar> litem(this->_array), end(this->_array); end.moveToEnd();
    ArrayIterator<Scalar> ritem(other.begin());
    for (; litem != end; litem++, ritem++) {
      (*litem) = (*ritem);
    }
    return *this;
  }

  ArrayValues<Scalar> &operator= (const ArrayValues<Scalar> &other) {
    ArrayIterator<Scalar> litem(this->_array), end(this->_array); end.moveToEnd();
    ArrayIterator<Scalar> ritem(other.begin());
    for (; litem != end; litem++, ritem++) {
      (*litem) = (*ritem);
    }
    return *this;
  }

protected:
  ArrayObj<Scalar> *_array;
};

}

#endif // __WT_ARRAYVALUES_HH__
