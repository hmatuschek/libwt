#ifndef UTILSTEST_HH
#define UTILSTEST_HH

#include "utils/unittest.hh"


class UtilsTest: public wt::UnitTest::TestCase
{
public:
  void testReadCSV();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // UTILSTEST_HH
