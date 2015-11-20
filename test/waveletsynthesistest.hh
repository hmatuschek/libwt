#ifndef WAVELETSYNTHESISTEST_HH
#define WAVELETSYNTHESISTEST_HH

#include "utils/unittest.hh"

class WaveletSynthesisTest : public wt::UnitTest::TestCase
{
public:
  void testSynthesis();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // WAVELETSYNTHESISTEST_HH
