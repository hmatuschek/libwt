#include "utils/unittest.hh"
#include <iostream>

#include "ffttest.hh"
#include "convolutiontest.hh"
#include "wavelettransformtest.hh"
#include "waveletsynthesistest.hh"
#include "waveletconvolutiontest.hh"


using namespace wt;
using namespace wt::UnitTest;


int main(int argc, char *argv[]) {

  // Construct test-runner
  TestRunner runner(std::cout);

  // Add suites
  runner.addSuite(FFTTest::suite());
  runner.addSuite(ConvolutionTest::suite());
  runner.addSuite(WaveletTransformTest::suite());
  runner.addSuite(WaveletSynthesisTest::suite());
  runner.addSuite(WaveletConvolutionTest::suite());

  // Exec tests:
  runner();

  return 0;
}
