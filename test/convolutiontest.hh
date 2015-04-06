#ifndef CONVOLUTIONTEST_HH
#define CONVOLUTIONTEST_HH

#include "types.hh"
#include "unittest.hh"
#include "convolution.hh"

class ConvolutionTest : public wt::UnitTest::TestCase
{
public:
  void testSingle();
  void testMultiple();

public:
  static wt::UnitTest::TestSuite *suite();
};

#endif // CONVOLUTIONTEST_HH
