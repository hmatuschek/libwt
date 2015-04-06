#ifndef __WT_OBJECT_HH__
#define __WT_OBJECT_HH__

#include <complex>
#include <cmath>


namespace wt {

class Object
{
public:
  Object();
  virtual ~Object();

  Object *ref();
  void unref();

protected:
  size_t _refCount;
};


class Container
{
public:
  typedef Object ObjectType;

public:
  Container();
  Container(Object *object);
  Container(const Container &other);
  virtual ~Container();

  Container &operator=(const Container &other);

  template <class T>
  inline bool is() {
    return 0 != dynamic_cast<typename T::ObjectType *>(_object);
  }

  template <class T>
  inline T as() {
    return T(dynamic_cast<typename T::ObjectType *>(_object));
  }

  /** Returns @c true if the container is empty (null). */
  inline bool isNull() const { return 0 == _object; }

protected:
  /** The object being managed. */
  Object *_object;
};

}

#endif // __WT_OBJECT_HH__
