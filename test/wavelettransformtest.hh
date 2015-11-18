#ifndef WAVELETTRANSDFORMTEST_HH
#define WAVELETTRANSDFORMTEST_HH

#include "unittest.hh"

class WaveletTransformTest : public wt::UnitTest::TestCase
{
public:
  void testTrafo();
  void testSubsample();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // WAVELETTRANSDFORMTEST_HH
