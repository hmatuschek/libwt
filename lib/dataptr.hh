#ifndef __WT_DATAPTR_HH__
#define __WT_DATAPTR_HH__

#include "types.hh"

namespace wt {

template <class T>
class DataPtrObj: public Object
{
public:
  typedef T Scalar;

protected:
  DataPtrObj(Scalar *data) : Object(), _data(data) { }

public:
  virtual ~DataPtrObj() { }
  inline Scalar *data() { return _data; }

protected:
  Scalar *_data;
};


template <class T>
class OwnedDataPtrObj: public DataPtrObj<T>
{
public:
  typedef T Scalar;

public:
  OwnedDataPtrObj(Scalar *data) : DataPtrObj<Scalar>(data) { }
  OwnedDataPtrObj(size_t N): DataPtrObj<Scalar>(new Scalar[N]) { }
  virtual ~OwnedDataPtrObj() { delete this->_data; }
};


template <class T>
class DataPtr: public Container
{
public:
  typedef T Scalar;
  typedef DataPtrObj<Scalar> ObjectType;

public:
  DataPtr() : Container(), _dataPtr(0) { }
  DataPtr(size_t N): Container(new OwnedDataPtrObj<Scalar>(N)),
    _dataPtr(static_cast<OwnedDataPtrObj<Scalar>*>(this->_object)) { }
  DataPtr(DataPtrObj<Scalar> *obj) : Container(obj), _dataPtr(obj) { }
  DataPtr(const DataPtr &other) : Container(other), _dataPtr(other._dataPtr) { }
  virtual ~DataPtr() { }

  DataPtr<Scalar> &operator=(const DataPtr &other) {
    Container::operator =(other);
    _dataPtr = other._dataPtr;
    return *this;
  }

  inline Scalar *data() const { return _dataPtr->data(); }

protected:
  DataPtrObj<Scalar> *_dataPtr;
};

}

#endif // __WT_DATAPTR_HH__
