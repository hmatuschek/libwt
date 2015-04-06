#ifndef __WT_ARRAYITERATOR_HH__
#define __WT_ARRAYITERATOR_HH__

#include "arrayobject.hh"


namespace wt {

/** Implements an iterator over the elements of an @c ArrayObject. */
template <class T>
class ArrayIterator: public Container
{
public:
  typedef T Scalar;
  typedef ArrayObj<Scalar> ObjectType;

public:
  ArrayIterator()
    : Container(), _array(0), _idxs(0)
  {
    // pass...
  }

  ArrayIterator(ArrayObj<Scalar> *array)
    : Container(array), _array(array), _idxs(new size_t[_array->rank()])
  {
    // Init indices
    for (size_t i=0; i<_array->rank(); i++) { _idxs[i] = 0; }
  }

  ArrayIterator(const ArrayIterator &other)
    : Container(other), _array(other._array), _idxs(new size_t[_array->rank()])
  {
    // Copy indices
    memcpy(_idxs, other._idxs, _array->rank()*sizeof(size_t));
  }

  virtual ~ArrayIterator() {
    if (0 != _idxs) { delete _idxs; }
  }

  void moveToEnd() {
    memcpy(_idxs, _array->dims(), _array->rank()*sizeof(size_t));
  }

  ArrayIterator &
  operator=(const ArrayIterator &other) {
    Container::operator =(other);
    _array = other._array;
    if (0 != _idxs) { delete _idxs; }
    _idxs = new size_t[_array->rank()];
    memcpy(_idxs, other._idxs, _array->rank()*sizeof(size_t));
    return *this;
  }

  bool
  operator ==(const ArrayIterator &other) const {
    if (_array != other._array) { return false; }
    for (size_t i=0; i<_array->rank(); i++) {
      if (_idxs[i] != other._idxs[i]) { return false; }
    }
    return true;
  }

  bool
  operator !=(const ArrayIterator &other) const {
    return !((*this) == other);
  }

  ArrayIterator &
  operator++(int) {
    for(size_t i=0; i<_array->rank(); i++) {
      _idxs[i]++;
      if (_idxs[i] < _array->dims()[i]) { return *this; }
      _idxs[i] = 0;
    }
    memcpy(_idxs, _array->dims(), _array->rank()*sizeof(size_t));
    return *this;
  }

  Scalar &operator*() {
    Scalar *p = _array->ptr();
    for (size_t i=0; i<_array->rank(); i++) { p += _idxs[i]*_array->strides()[i]; }
    return *p;
  }

  const Scalar &operator*() const {
    Scalar *p = _array->ptr();
    for (size_t i=0; i<_array->rank(); i++) { p += _idxs[i]*_array->strides()[i]; }
    return *p;
  }

protected:
  ArrayObj<Scalar> *_array;
  size_t *_idxs;
};


}


#endif // __WT_ARRAYITERATOR_HH__
