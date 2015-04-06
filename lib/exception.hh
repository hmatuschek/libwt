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

#define ASSERT_DIM_EQUAL(a,b) \
  if (a!=b) { wt::Error err; err << "Assertion failed: " << #a << " != " << #b; throw err; }

}

#define ASSERT_SAME_SHAPE(a, b) { \
  if (a.rank() != b.rank()) { \
  wt::Error err; err << "Shape mismatch: rank(" << #a << ") != rank(" << #b << ")"; throw err; \
  }\
  for (size_t i=0; i<a.rank(); i++) { \
  if (a.dims()[i] != b.dims()[i]) { \
  wt::Error err; err << "Shape mismatch: dim(" << #a << ", " << i <<\
  ") != shape(" << #b << ", " << i << ")"; throw err; \
  }\
  }\
  }

#endif // __WT_EXCEPTION_HH__
