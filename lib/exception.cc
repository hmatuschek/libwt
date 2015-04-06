#include "exception.hh"

using namespace wt;

Error::Error()
  : std::exception(), std::stringstream()
{
  // pass...
}

Error::Error(const Error &other)
  : std::exception(), std::stringstream(other.str())
{
  // pass...
}

Error::Error(const std::string &msg)
  : std::exception(), std::stringstream(msg)
{
  // pass...
}

Error::~Error() throw () {
  // pass...
}

const char *
Error::what() const throw () {
  return this->str().c_str();
}
