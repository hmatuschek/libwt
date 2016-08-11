#include "waveletconvolutiontest.hh"
#include "wavelettransform.hh"
#include "waveletconvolution.hh"
#include <iostream>

using namespace wt;

void
WaveletConvolutionTest::testConvolution() {
  // Delta peak
  int N=512;
  int Nscales = 64;
  Eigen::VectorXcd signal = Eigen::VectorXcd::Zero(N); signal(N/2) = 1;
  // Perform WT of delta-peak on single scale
  //  -> evaluation of wavelet at that scale
  Eigen::VectorXd scales(Nscales); scales[0] = 2.;
  double ds = 100./Nscales;
  for (int i=1; i<Nscales; i++) {
    scales[i] = scales[i-1]+ds;
  }

  Eigen::MatrixXcd transformed(N, Nscales);
  GenericWaveletTransform<double> wt(Cauchy(16), scales);
  wt(signal, transformed);

  Eigen::MatrixXcd proj(N, Nscales);
  GenericWaveletConvolution<double> P(wt);
  P(transformed, proj);

  // Do not test anything, its bad anyway
}



UnitTest::TestSuite *
WaveletConvolutionTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Wavelet Convolution Test");

  suite->addTest(new UnitTest::TestCaller<WaveletConvolutionTest>(
                   "auto-proj.", &WaveletConvolutionTest::testConvolution));

  return suite;
}
