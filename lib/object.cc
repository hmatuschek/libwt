#include "object.hh"

using namespace wt;

Object::Object()
  : _refCount(0)
{
  // pass...
}

Object::~Object() {
  // pass...
}

Object *
Object::ref() {
  _refCount++;
  return this;
}

void
Object::unref() {
  _refCount--;
  if (0 == _refCount) {
    delete this;
  }
}


Container::Container()
  : _object(0)
{
  // pass...
}

Container::Container(Object *obj)
  : _object(obj)
{
  if (_object) { _object->ref(); }
}

Container::Container(const Container &other)
  : _object(other._object)
{
  if (_object) { _object->ref(); }
}

Container::~Container() {
  if (_object) { _object->unref(); }
}

Container &
Container::operator =(const Container &other) {
  if (_object) { _object->unref(); }
  _object = other._object;
  if (_object) { _object->ref(); }
  return *this;
}
