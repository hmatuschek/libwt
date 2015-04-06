#ifndef __WT_ARRAY_HH__
#define __WT_ARRAY_HH__

#include "arrayobject.hh"
#include "arrayiterator.hh"
#include "arrayvalues.hh"

namespace wt {

/** Container class for array objects. */
template <class T>
class Array : public Container
{
public:
  typedef T Scalar;
  typedef ArrayObj<Scalar> ObjectType;

public:
  Array()
    : Container(), _array(0)
  { }
  Array(ArrayObj<Scalar> *obj)
    : Container(obj), _array(obj)
  { }
  Array(const Array<Scalar> &other)
    : Container(other), _array(other._array)
  { }
  Array(size_t N)
    : Container(new ArrayObj<Scalar>(N)), _array(static_cast<ArrayObj<Scalar>*>(_object))
  { }
  Array(size_t N, size_t M)
    : Container(new ArrayObj<Scalar>(N, M)), _array(static_cast<ArrayObj<Scalar>*>(_object))
  { }
  Array(size_t N, size_t M, size_t L)
    : Container(new ArrayObj<Scalar>(N, M, L)), _array(static_cast<ArrayObj<Scalar>*>(_object))
  { }
  Array(size_t rank, const size_t dims[])
    : Container(new ArrayObj<Scalar>(rank, dims)), _array(static_cast<ArrayObj<Scalar>*>(_object))
  { }
  virtual ~Array()
  { }

  inline Array &operator=(const Array<Scalar> &other) {
    Container::operator =(other);
    _array = other._array;
    return *this;
  }

  inline Scalar &at(size_t idxs[]) {
    Scalar *p = _array->ptr();
    for (size_t i=0; i<_array->_rank; i++) { p += idxs[i]*_array->strides()[i]; }
    return *p;
  }

  inline const Scalar &at(size_t idxs[]) const {
    Scalar *p = _array->ptr();
    for (size_t i=0; i<_array->_rank; i++) { p += idxs[i]*_array->strides()[i]; }
    return *p;
  }

  inline size_t offset() const { return _array->offset(); }
  inline size_t rank() const { return _array->rank(); }
  inline const size_t* dims() const { return _array->dims(); }
  inline const size_t* strides() const { return _array->strides(); }
  inline Scalar *ptr() const { return _array->ptr(); }

  inline ArrayValues<Scalar> values() const { return ArrayValues<Scalar>(_array); }

  inline ArrayIterator<Scalar> begin() const {
    return ArrayIterator<Scalar>(this->_array);
  }

  inline ArrayIterator<Scalar> end() const {
    ArrayIterator<Scalar> iter(this->_array); iter.moveToEnd();
    return iter;
  }

protected:
  ArrayObj<Scalar> *_array;
};


/** Specialized container class for vectors. */
template <class T>
class Vector: public Array<T>
{
public:
  typedef T Scalar;
  typedef ArrayObj<Scalar> ObjectType;

public:
  Vector() : Array<Scalar>() { }
  explicit Vector(size_t N) : Array<Scalar>(N) { }
  explicit Vector(ArrayObj<Scalar> *obj) : Array<Scalar>(obj) { ASSERT_DIM_EQUAL(obj->rank(), 1); }
  explicit Vector(const Array<Scalar> &array): Array<Scalar>(array) { ASSERT_DIM_EQUAL(array.rank(), 1); }
  Vector(const Vector<Scalar> &array): Array<Scalar>(array) { }
  virtual ~Vector() { }

  inline Vector &operator=(const Vector<Scalar> &other) {
    Array<Scalar>::operator =(other);
    return *this;
  }

  inline Scalar &operator[](size_t idx) {
    size_t idxs[1] = {idx}; return at(idxs);
  }

  inline const Scalar &operator[](size_t idx) const {
    size_t idxs[1] = {idx}; return at(idxs);
  }

  inline Scalar &at(size_t idx) {
    size_t idxs[1] = {idx}; return at(idxs);
  }

  inline const Scalar &at(size_t idx) const {
    size_t idxs[1] = {idx}; return at(idxs);
  }

  inline size_t dim() const { return this->_array->dims()[0]; }

  Vector<Scalar> head(size_t N) const {
    size_t offset  = this->_array->offset();
    size_t dims[1] = { N };
    return Vector<Scalar>(
          new ArrayObj<Scalar>(
            this->_array, offset, 1, dims, this->_array->strides()));
  }

  Vector<Scalar> tail(size_t i) const {
    size_t offset  = this->_array->offset() + i*this->strides()[0];
    size_t dims[1] = { this->dim()-i };
    return Vector<Scalar>(
          new ArrayObj<Scalar>(
            this->_array, offset, 1, dims, this->_array->strides()));
  }

  Vector<Scalar> sub(size_t i, size_t N) const {
    size_t offset  = this->_array->offset() + i*this->strides()[0];
    size_t dims[1] = { N };
    return Vector<Scalar>(
          new ArrayObj<Scalar>(
            this->_array, offset, 1, dims, this->_array->strides()));
  }
};


/** Specialized container class for matrices. */
template <class T>
class Matrix: public Array<T>
{
public:
  typedef T Scalar;
  typedef ArrayObj<Scalar> ObjectType;

public:
  Matrix(): Array<Scalar>() { }
  Matrix(size_t N, size_t M): Array<Scalar>(N,M) { }
  explicit Matrix(ArrayObj<Scalar> *obj): Array<Scalar>(obj) {
    if (! this->isNull()) { ASSERT_DIM_EQUAL(obj->rank(), 2); }
  }
  explicit Matrix(const Array<Scalar> &obj): Array<Scalar>(obj) {
    if (! this->isNull()) { ASSERT_DIM_EQUAL(obj->rank(), 2); }
  }
  Matrix(const Matrix<Scalar> &other) : Array<Scalar>(other) { }
  virtual ~Matrix() { }

  inline Matrix &operator=(const Matrix<Scalar> &other) {
    Array<Scalar>::operator =(other);
    return *this;
  }

  inline Scalar &at(size_t row, size_t col) {
    size_t idxs[2] = {row, col}; return this->at(idxs);
  }

  inline const Scalar &at(size_t row, size_t col) const {
    size_t idxs[2] = {row, col}; return this->at(idxs);
  }

  inline size_t rows() const { return this->_array->dims()[0]; }
  inline size_t rowStride() const { return this->_array->strides()[0]; }

  inline size_t cols() const { return this->_array->dims()[1]; }
  inline size_t colStride() const { return this->_array->strides()[1]; }

  inline Vector<Scalar> col(size_t j) const {
    // Compute offset
    size_t offset     = this->_array->offset() + j*this->_array->strides()[1];
    size_t dims[1]    = { this->rows() };
    size_t strides[1] = { this->rowStride() };
    return Vector<Scalar>(new ArrayObj<Scalar>(this->_array, offset, 1, dims, strides));
  }

  inline Vector<Scalar> row(size_t i) const {
    size_t offset     = this->_array->offset() + i*this->_array->strides()[0];
    size_t dims[1]    = { this->cols() };
    size_t strides[1] = { this->colStride() };
    return Vector<Scalar>(new ArrayObj<Scalar>(this->_array, offset, 1, dims, strides));
  }

  inline Matrix<Scalar> sub(size_t i, size_t j, size_t N, size_t M) {
    size_t offset     = this->_array->offset() + i*this->_array->strides()[0]
        + j*this->_array->strides()[1];
    size_t dims[2]    = { N,M };
    return Matrix<Scalar>(new ArrayObj<Scalar>(this->_array, offset, 2,
                                               dims, this->_array->strides()));
  }
};

}

#endif // __WT_ARRAY_HH__
