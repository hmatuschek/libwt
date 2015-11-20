#include "convolutiontest.hh"
#include <iostream>

using namespace wt;

void
ConvolutionTest::testSingle() {
  Eigen::VectorXcd in(16);
  for (size_t i=0; i<16; i++) { in(i) = std::sin(2*M_PI*i/8); }
  Eigen::VectorXcd kernel = Eigen::VectorXcd::Zero(6); kernel(3) = 1;

  Convolution<double> conv(kernel);
  Eigen::VectorXcd out = Eigen::VectorXcd::Zero(16);
  conv.apply(in, out);
  for (size_t i=0; i<16; i++) {
    UT_ASSERT_NEAR_EPS(out(i).real(), in(i).real(), 1e-8);
    UT_ASSERT_NEAR_EPS(out(i).imag(), in(i).imag(), 1e-8);
  }
}

void
ConvolutionTest::testMultiple() {
  Eigen::VectorXcd in(16);
  for (size_t i=0; i<16; i++) { in(i) = std::sin(2*M_PI*i/8); }
  Eigen::MatrixXcd kernel = Eigen::MatrixXcd::Zero(6,2); kernel.row(3).setConstant(1);

  Convolution<double> conv(kernel);
  Eigen::MatrixXcd out = Eigen::MatrixXcd::Zero(16,2);
  conv.apply(in, out);

  for (size_t i=0; i<16; i++) {
    UT_ASSERT_NEAR_EPS(out(i,0).real(), in(i).real(), 1e-8);
    UT_ASSERT_NEAR_EPS(out(i,0).real(), in(i).real(), 1e-8);
    UT_ASSERT_NEAR_EPS(out(i,1).real(), in(i).real(), 1e-8);
    UT_ASSERT_NEAR_EPS(out(i,1).real(), in(i).real(), 1e-8);
  }
}

void
ConvolutionTest::testShortSignal() {
  Eigen::VectorXcd in(16);
  for (size_t i=0; i<16; i++) { in(i) = std::sin(2*M_PI*i/8); }
  Eigen::MatrixXcd kernel = Eigen::MatrixXcd::Zero(32,1);
  kernel.row(16).setConstant(1);

  Convolution<double> conv(kernel);
  Eigen::MatrixXcd out = Eigen::MatrixXcd::Zero(16,1);
  conv.apply(in, out);

  for (size_t i=0; i<16; i++) {
    UT_ASSERT_NEAR_EPS(out(i,0).real(), in(i).real(), 1e-6);
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
  suite->addTest(new UnitTest::TestCaller<ConvolutionTest>(
                   "short signal", &ConvolutionTest::testShortSignal));

  return suite;
}
