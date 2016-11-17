#include "utilstest.hh"
#include "utils/csv.hh"
using namespace wt;


void
UtilsTest::testReadCSV() {
  std::stringstream buffer;
  buffer << "1.00\t1e-0\n"
         << "20.0\t2E+1\n"
         << "0.02\t2e-2\n";

  Eigen::MatrixXd res;
  CSV::read(res, buffer);

  UT_ASSERT_EQUAL(res.rows(), long(3));
  UT_ASSERT_EQUAL(res.cols(), long(2));
  for (int i=0; i<res.rows(); i++) {
    UT_ASSERT_EQUAL(res(i,0), res(i,1));
    UT_ASSERT_EQUAL(res(i,0), res(i,1));
  }
}

UnitTest::TestSuite *
UtilsTest::suite() {
  UnitTest::TestSuite *suite = new UnitTest::TestSuite("Utilities");
  suite->addTest(new UnitTest::TestCaller<UtilsTest>("CSV::read", &UtilsTest::testReadCSV));
  return suite;
}
