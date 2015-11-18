#ifndef FFTTEST_HH
#define FFTTEST_HH

#include "utils/unittest.hh"
#include "fft.hh"


class FFTTest: public wt::UnitTest::TestCase
{
public:
  void testVector();
  void testMatrix();

  static wt::UnitTest::TestSuite *suite();
};

#endif // FFTTEST_HH
