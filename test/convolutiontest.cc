#include "convolutiontest.hh"
#include <iostream>

using namespace wt;

void
ConvolutionTest::testSingle() {
  CVector in(16);
  for (size_t i=0; i<16; i++) { in(i) = std::sin(2*M_PI*i/8); }
  CVector kernel = CVector::Zero(5); kernel(2) = 1;

  Convolution conv(kernel);
  CVector out = CVector::Zero(16);
  conv.apply(in, out);
  for (size_t i=0; i<16; i++) {
    UT_ASSERT_NEAR_EPS(out(i).real(), in(i).real(), 1e-9);
    UT_ASSERT_NEAR_EPS(out(i).imag(), in(i).imag(), 1e-9);
  }
}

void
ConvolutionTest::testMultiple() {
  CVector in(16);
  for (size_t i=0; i<16; i++) { in(i) = std::sin(2*M_PI*i/8); }
  CMatrix kernel = CMatrix::Zero(5,2); kernel.row(2).setConstant(1);

  Convolution conv(kernel);
  CMatrix out = CMatrix::Zero(16,2);
  conv.apply(in, out);

  for (size_t i=0; i<16; i++) {
    UT_ASSERT_NEAR_EPS(out(i,0).real(), in(i).real(), 1e-9);
    UT_ASSERT_NEAR_EPS(out(i,0).real(), in(i).real(), 1e-9);
    UT_ASSERT_NEAR_EPS(out(i,1).real(), in(i).real(), 1e-9);
    UT_ASSERT_NEAR_EPS(out(i,1).real(), in(i).real(), 1e-9);
  }
}

UnitTest::TestSuite *
ConvolutionTest::suite()
{
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Convolution Test");

  suite->addTest(new UnitTest::TestCaller<ConvolutionTest>(
                   "single filter", &ConvolutionTest::testSingle));
  suite->addTest(new UnitTest::TestCaller<ConvolutionTest>(
                   "multiple filter", &ConvolutionTest::testMultiple));

  return suite;
}
