#include "wavelettransformtest.hh"
#include "wavelettransform.hh"
#include <iostream>

using namespace wt;

void
WaveletTransformTest::testTrafo() {
  // Delta peak
  size_t N=1024;
  CVector signal = CVector::Zero(N); signal(N/2) = 1;
  // Perform WT of delta-peak on single scale
  //  -> evaluation of wavelet at that scale
  RVector scales(1); scales(0) = 100;
  CMatrix transformed(N, 1);

  WaveletTransform wt(Morlet(), scales);
  wt(signal, transformed);
  for (size_t i=0; i<N; i++) {
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).real(), wt.wavelet().eval((double(i)-N/2)/100).real()/100, 1e-9);
    UT_ASSERT_NEAR_EPS(
          transformed(i,0).imag(), wt.wavelet().eval((double(i)-N/2)/100).imag()/100, 1e-9);
  }
}


UnitTest::TestSuite *
WaveletTransformTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Wavelet Transform Test");

  suite->addTest(new UnitTest::TestCaller<WaveletTransformTest>(
                   "trafo", &WaveletTransformTest::testTrafo));

  return suite;
}
