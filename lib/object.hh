/** @defgroup container Memory management
 * For the ease of providing interfaces to several programming languages, all objects of this
 * library which require to be passed around as references provide container classes (derived from
 * @c Container). These classes provide access to the actual instances (@c derived from @c Object)
 * and manage these references. They free the insances once the last reference to them is deleted.
 * This allows the programmer to pass these objects around like values.
 */

#ifndef __WT_OBJECT_HH__
#define __WT_OBJECT_HH__

#include <complex>
#include <cmath>


namespace wt {

/** Base class of all managed objects.
 * @ingroup container */
class Object
{
protected:
  /** Hidden constructor. */
  Object();

public:
  /** Destructor. */
  virtual ~Object();

  /** Retunrs a new reference to the object (increases the reference counter). */
  Object *ref();
  /** Invalidates this reference to the object (decreases the reference counter). */
  void unref();

protected:
  /** The reference counter. */
  size_t _refCount;
};


/** Base class of all container classes holding a managed reference to some @c Object.
 * @ingroup container */
class Container
{
public:
  /** Specifies the object type being held by the container class. */
  typedef Object ObjectType;

public:
  /** Empty constructor. */
  Container();
  /** Packs the given object in a container. The ownership of the object is taken. */
  Container(Object *object);
  /** Copy constructor. */
  Container(const Container &other);
  /** Destructor. */
  virtual ~Container();
  /** Assignment operator. */
  Container &operator=(const Container &other);

  /** Returns @c true if the object held by the container is of type @c T::ObjectType. */
  template <class T>
  inline bool is() {
    return 0 != dynamic_cast<typename T::ObjectType *>(_object);
  }

  /** Container cast. */
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
