#ifndef __WT_ARRAYOBJECT_HH__
#define __WT_ARRAYOBJECT_HH__

#include "object.hh"
#include "dataptr.hh"
#include "exception.hh"


namespace wt {

/** Implements a N-dimensional view on a @c DataPtr. */
template <class T>
class ArrayObj: public Object
{
public:
  typedef T Scalar;

public:
  ArrayObj(const DataPtr<Scalar> &ptr, size_t offset, size_t rank, const size_t dims[], const size_t strides[])
    : Object(), _ptr(ptr), _offset(offset), _rank(rank),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    memcpy(_dims, dims, _rank*sizeof(size_t));
    memcpy(_strides, strides, _rank*sizeof(size_t));
  }

  ArrayObj(const ArrayObj *array, size_t offset, size_t rank, const size_t dims[], const size_t strides[])
    : Object(), _ptr(array->_ptr), _offset(offset), _rank(rank),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    memcpy(_dims, dims, _rank*sizeof(size_t));
    memcpy(_strides, strides, _rank*sizeof(size_t));
  }

  ArrayObj(size_t rank, const size_t dims[])
    : Object(), _ptr(), _offset(0), _rank(rank),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    size_t N = 1;
    for (size_t i=0; i<_rank; i++) { N *= dims[i]; }
    // Allocate data
    _ptr = DataPtr<Scalar>(N);
    // copy dimensions
    memcpy(_dims, dims, _rank*sizeof(size_t));
    // assemble strides (column major)
    _strides[0] = 1;
    for (int i=1; i<_rank; i++) {
      _strides[i] = _strides[i-1]*_dims[i-1];
    }
  }

  ArrayObj(size_t N)
    : Object(), _ptr(N), _offset(0), _rank(1),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    _dims[0]    = N;
    _strides[0] = 1;
  }

  ArrayObj(size_t N, size_t M)
    : Object(), _ptr(N*M), _offset(0), _rank(2),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    _dims[0]    = N; _dims[1]    = M;
    _strides[0] = 1; _strides[1] = N;
  }

  ArrayObj(size_t N, size_t M, size_t L)
    : Object(), _ptr(N*M*L), _offset(0), _rank(3),
      _dims(new size_t[_rank]), _strides(new size_t[_rank])
  {
    _dims[0]    = N; _dims[1]    = M; _dims[2]    = L;
    _strides[0] = 1; _strides[1] = N; _strides[1] = N*M;
  }

  virtual ~ArrayObj() {
    delete _dims; delete _strides;
  }

  inline size_t offset() const { return _offset; }
  inline size_t rank() const { return _rank; }
  inline const size_t* dims() const { return _dims; }
  inline const size_t* strides() const { return _strides; }

  inline Scalar *data() const { return _ptr.data(); }
  inline Scalar *ptr() const { return _ptr.data()+_offset; }

protected:
  DataPtr<Scalar> _ptr;
  size_t _offset;
  size_t _rank;
  size_t *_dims;
  size_t *_strides;
};

}


#endif // __WT_ARRAYOBJECT_HH__
