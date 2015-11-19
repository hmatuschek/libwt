#ifndef CONVOLUTIONTEST_HH
#define CONVOLUTIONTEST_HH

#include "types.hh"
#include "utils/unittest.hh"
#include "convolution.hh"

class ConvolutionTest : public wt::UnitTest::TestCase
{
public:
  void testSingle();
  void testMultiple();
  void testShortSignal();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // CONVOLUTIONTEST_HH
