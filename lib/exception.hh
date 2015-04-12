#ifndef __WT_EXCEPTION_HH__
#define __WT_EXCEPTION_HH__

#include <sstream>
#include <exception>

namespace wt {

/** Base class of all exceptions. */
class Error: public std::exception, public std::stringstream
{
public:
  /** Empty constructor. */
  Error();
  /** Constructor with message. */
  Error(const std::string &msg);
  /** Copy constructor. */
  Error(const Error &other);
  /** Destructor. */
  virtual ~Error() throw();
  /** Returns the message of the exception. */
  virtual const char *what() const throw();
};

}

#endif // __WT_EXCEPTION_HH__
