#include "wavelettransformtest.hh"
#include "wavelettransform.hh"
#include <iostream>

using namespace wt;

void
WaveletTransformTest::testTrafo() {
  // Delta peak
  int N=128*1024;
  double scale = 200;
  int Nscales = 10;
  Eigen::VectorXcd signal = Eigen::VectorXcd::Zero(N); signal(N/2) = 1;
  // Perform WT of delta-peak on single scale
  //  -> evaluation of wavelet at that scale
  Eigen::VectorXd scales(Nscales); scales.setConstant(scale);
  Eigen::MatrixXcd transformed(N, Nscales);

  WaveletTransform<double> wt(Morlet(), scales);
  wt(signal, transformed);
  for (int i=0; i<N; i++) {
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).real(), wt.wavelet().evalAnalysis(double(i-N/2)/scale).real()/scale, 1e-5);
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).imag(), wt.wavelet().evalAnalysis(double(i-N/2)/scale).imag()/scale, 1e-5);
  }
}


void
WaveletTransformTest::testSubsample() {
  // Delta peak
  int N=128*1024;
  int Nscales = 10;
  double scale = 200;
  Eigen::VectorXcd signal = Eigen::VectorXcd::Zero(N); signal(N/2) = 1;
  // Perform WT of delta-peak on single scale
  //  -> evaluation of wavelet at that scale
  Eigen::VectorXd scales(Nscales); scales.setConstant(scale);
  Eigen::MatrixXcd transformed(N, Nscales);

  WaveletTransform<double> wt(Morlet(), scales, true);
  wt(signal, transformed);
  for (int i=0; i<N; i++) {
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).real(), wt.wavelet().evalAnalysis(double(i-N/2)/scale).real()/scale, 1e-2);
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).imag(), wt.wavelet().evalAnalysis(double(i-N/2)/scale).imag()/scale, 1e-2);
  }
}


UnitTest::TestSuite *
WaveletTransformTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Wavelet Transform Test");

  suite->addTest(new UnitTest::TestCaller<WaveletTransformTest>(
                   "trafo", &WaveletTransformTest::testTrafo));
  suite->addTest(new UnitTest::TestCaller<WaveletTransformTest>(
                   "subsample", &WaveletTransformTest::testSubsample));

  return suite;
}
