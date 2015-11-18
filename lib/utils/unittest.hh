#ifndef __WT_UNITTEST_HH__
#define __WT_UNITTEST_HH__

#include <list>
#include <string>
#include <limits>
#include <cmath>
#include <sstream>

namespace wt {
namespace UnitTest {

/**
 * Base exception to signal the failure of a test-case.
 * @ingroup unittest
 */
class TestFailure : public std::exception
{
protected:
  /** Holds the error message. */
  std::string message;

public:
  /** Constructor. */
  TestFailure(const std::string &message) throw();

  /** Destructor. */
  virtual ~TestFailure() throw();

  /** Returns the message as a char pointer. */
  const char *what() const throw();
};



/**
 * Base class of all test-cases.
 * @ingroup unittest
 */
class TestCase
{
public:
  /** Will be called just before each test. */
  virtual void setUp();

  /** Will be called just after each test. */
  virtual void tearDown();

  /** Asserts, the given expression @c test is true. */
  void assertTrue(bool test, const std::string &file, size_t line);

  /** Asserts, the two values are equal. */
  template <class T>
  void assertEqual(const T &t, const T &e, const std::string &file, size_t line) {
    if (e != t) {
      std::stringstream str;
      str << "Expected: \"" << e << "\" but got: \"" << t << "\""
          << " in file "<< file << " in line " << line;
      throw TestFailure(str.str());
    }
  }

  /** Asserts, the two values are almost equal. */
  template <class T>
  void assertNear(const T &t, const T &e, const T &eps, const std::string &file, size_t line) {
    if (std::abs(e-t) > eps) {
      std::stringstream str;
      str << "Expected: " << e << " but got: " << t
          << " in file "<< file << " in line " << line;
      throw TestFailure(str.str());
    }
  }

  /** Asserts, the two values are almost equal. */
  template <class T>
  void assertNear(const T &t, const T &e, const std::string &file, size_t line) {
    if (std::abs(e-t) > std::numeric_limits<T>::epsilon()) {
      std::stringstream str;
      str << "Expected: " << e << " but got: " << t
          << " in file "<< file << " in line " << line;
      throw TestFailure(str.str());
    }
  }

};



/**
 * Base interface for tests, internal used.
 * @ingroup unittest
 */
class TestCallerInterface
{
protected:
  /** Description of the test. */
  std::string description;

public:
  /** Constructor. */
  TestCallerInterface(const std::string &desc);

  /** Virtual destructor. */
  virtual ~TestCallerInterface();

  /** Returns the description of the test. */
  virtual const std::string &getDescription() const;

  /** Needs to be overridded to execute the test. */
  virtual void operator() () = 0;
};



/**
 * Template for test as methods of a @c TestCase.
 * @ingroup unittest
 */
template <class T>
class TestCaller : public TestCallerInterface
{
protected:
  /** Pointer to a member function of template-class. */
  void (T::*function)(void);


public:
  /** Constructor. */
  TestCaller(const std::string &desc, void (T::*func)(void))
    : TestCallerInterface(desc), function(func)
  {
    // Pass...
  }

  /** Calls the referenced test-function. */
  virtual void operator() ()
  {
    // Create new test:
    T *instance = new T();

    // Call test
    instance->setUp();
    (instance->*function)();
    instance->tearDown();
  }
};


/**
 * A collection of tests.
 * @ingroup unittest
 */
class TestSuite
{
public:
  /** Defines the interator over all tests. */
  typedef std::list<TestCallerInterface *>::iterator iterator;

protected:
  /** Description of the test-suite. */
  std::string description;

  /** Holds the list of tests. */
  std::list<TestCallerInterface *> tests;

public:
  /** Constructor. */
  TestSuite(const std::string &desc);

  /** Adds a test. */
  void addTest(TestCallerInterface *test);

  /** Returns the description of the test-suite. */
  const std::string &getDescription();

  /** Returns an iterator pointing to the first test. */
  iterator begin();

  /** Returns an interator pointing right after the last test. */
  iterator end();
};



/**
 * Runs tests of one or more @c TestSuite instances.
 * @ingroup unittest
 */
class TestRunner
{
protected:
  /** Output stream. */
  std::ostream &stream;
  /** A list of test suites. */
  std::list<TestSuite *> suites;

public:
  /** Constructor. */
  TestRunner(std::ostream &stream);

  /** Adds a test-suite. */
  void addSuite(TestSuite *suite);
  /** Executes all test-suites. */
  void operator() ();
};


/** Asserts that the given expression is true.
 * @ingroup unittest */
#define UT_ASSERT(t) this->assertTrue(t, __FILE__, __LINE__)
/** Asserts that the given values are equal.
 * @ingroup unittest */
#define UT_ASSERT_EQUAL(t, e) this->assertEqual(t, e, __FILE__, __LINE__)
/** Asserts that the given values are almost equal.
 * @ingroup unittest */
#define UT_ASSERT_NEAR(t, e) this->assertNear(t, e, __FILE__, __LINE__)
/** Asserts that the given values are almost equal (up to given precision).
 * @ingroup unittest */
#define UT_ASSERT_NEAR_EPS(t, e, eps) this->assertNear(t, e, eps, __FILE__, __LINE__)
/** Asserts that a certain exception will be thrown.
 * @ingroup unittest */
#define UT_ASSERT_THROW(t, e) \
  try { t; throw UnitTest::TestFailure("No exception thrown!"); } catch (e &err) {}

}
}

#endif // UNITTEST_HH
