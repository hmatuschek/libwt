#include "ffttest.hh"

using namespace wt;


void
FFTTest::testVector() {
  fftw_complex in[8] = {{1,0},{1,0},{0,0},{0,0},{1,0},{1,0},{0,0},{0,0}}, out[8];
  fftw_plan plan = fftw_plan_dft_1d(8, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  Eigen::Matrix<std::complex<double>, Eigen::Dynamic, 1> ein(8), eout(8);
  ein << 1,1,0,0,1,1,0,0;
  eout.setZero();
  FFT::exec(ein, eout, FFT::FORWARD);

  for (int i=0; i<8; i++) {
    UT_ASSERT_EQUAL(out[i][0], eout[i].real());
    UT_ASSERT_EQUAL(out[i][1], eout[i].imag());
  }
}


void
FFTTest::testMatrix() {
  fftw_complex in[4] = {{1,0},{1,0},{0,0},{0,0}}, out[4];
  fftw_plan plan = fftw_plan_dft_1d(4, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);

  Eigen::Matrix<std::complex<double>, Eigen::Dynamic, Eigen::Dynamic> ein(4,4), eout(4,4);
  ein.col(0) << 1,1,0,0;
  ein.col(1) << 1,1,0,0;
  ein.col(2) << 1,1,0,0;
  ein.col(3) << 1,1,0,0;
  eout.setZero();
  FFT::exec(ein, eout, FFT::FORWARD);

  for (int j=0; j<4; j++) {
    for (int i=0; i<4; i++) {
      UT_ASSERT_EQUAL(out[i][0], eout(i,j).real());
      UT_ASSERT_EQUAL(out[i][1], eout(i,j).imag());
    }
  }
}


UnitTest::TestSuite *
FFTTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("FFT interface");
  suite->addTest(new UnitTest::TestCaller<FFTTest>("Vector", &FFTTest::testVector));
  suite->addTest(new UnitTest::TestCaller<FFTTest>("Matrix", &FFTTest::testMatrix));
  return suite;
}
