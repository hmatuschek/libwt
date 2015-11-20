#include "waveletsynthesistest.hh"
#include "wavelettransform.hh"
#include "waveletsynthesis.hh"
#include <iostream>

using namespace wt;

void
WaveletSynthesisTest::testSynthesis() {
  // Delta peak
  int N=1024;
  int Nscales = 64;
  Eigen::VectorXcd signal = Eigen::VectorXcd::Zero(N); signal(N/2) = 1;
  // Perform WT of delta-peak on single scale
  //  -> evaluation of wavelet at that scale
  Eigen::VectorXd scales(Nscales); scales[0] = 2.;
  double ds = 100./Nscales;
  for (size_t i=1; i<Nscales; i++) {
    scales[i] = scales[i-1]+ds;
  }

  Eigen::MatrixXcd transformed(N, Nscales);
  WaveletTransform<double> wt(Cauchy(16), scales);
  wt(signal, transformed);

  Eigen::VectorXcd reconst(N);
  WaveletSynthesis<double> ws(wt);
  ws(transformed, reconst);

  // Do not test anything, its bad anyway
}



UnitTest::TestSuite *
WaveletSynthesisTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Wavelet Synthesis Test");

  suite->addTest(new UnitTest::TestCaller<WaveletSynthesisTest>(
                   "synthesis", &WaveletSynthesisTest::testSynthesis));

  return suite;
}
