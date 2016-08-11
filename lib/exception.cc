#include "exception.hh"

using namespace wt;


/* ********************************************************************************************* *
 * Implementation of Error base exception
 * ********************************************************************************************* */
Error::Error()
  : std::exception(), std::stringstream()
{
  // pass...
}

Error::Error(const std::string &msg)
  : std::exception(), std::stringstream()
{
  (*this) << msg;
}

Error::Error(const Error &other)
  : std::exception(), std::stringstream()
{
  (*this) << other.str();
}

Error::~Error() throw() {
  // pass...
}

const char *
Error::what() const throw () {
  return this->str().c_str();
}


/* ********************************************************************************************* *
 * Implementation of DimensionError exception
 * ********************************************************************************************* */
DimensionError::DimensionError()
  : Error()
{
  // pass...
}

DimensionError::DimensionError(const std::string &msg)
  : Error(msg)
{
  (*this) << msg;
}

DimensionError::DimensionError(const DimensionError &other)
  : Error(other)
{
  // pass...
}

DimensionError::~DimensionError() throw() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of IOError exception
 * ********************************************************************************************* */
IOError::IOError()
  : Error()
{
  // pass...
}

IOError::IOError(const std::string &msg)
  : Error(msg)
{
  (*this) << msg;
}

IOError::IOError(const IOError &other)
  : Error(other)
{
  // pass...
}

IOError::~IOError() throw() {
  // pass...
}


/* ********************************************************************************************* *
 * Implementation of ValueError exception
 * ********************************************************************************************* */
ValueError::ValueError()
  : Error()
{
  // pass...
}

ValueError::ValueError(const std::string &msg)
  : Error(msg)
{
  (*this) << msg;
}

ValueError::ValueError(const ValueError &other)
  : Error(other)
{
  // pass...
}

ValueError::~ValueError() throw() {
  // pass...
}


