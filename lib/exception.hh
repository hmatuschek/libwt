#ifndef __WT_EXCEPTION_HH__
#define __WT_EXCEPTION_HH__

#include <sstream>
#include <exception>

namespace wt {

/** Base class of all exceptions. */
class Error: public std::exception, public std::stringstream
{
public:
  Error();
  Error(const std::string &msg);
  Error(const Error &other);

  virtual ~Error() throw();

  virtual const char *what() const throw();
};

}

#endif // __WT_EXCEPTION_HH__
