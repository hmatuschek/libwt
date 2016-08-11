#ifndef WAVELETCONVOLUTIONTEST_HH
#define WAVELETCONVOLUTIONTEST_HH

#include "utils/unittest.hh"

class WaveletConvolutionTest : public wt::UnitTest::TestCase
{
public:
  void testConvolution();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // WAVELETCONVOLUTIONTEST_HH
