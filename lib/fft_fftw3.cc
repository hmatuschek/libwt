#include "fft_fftw3.hh"
#include "exception.hh"
#include <iostream>

using namespace wt;


FFT::FFT(CVector &in, CVector &out, Direction dir)
{
  if (in.rows() != out.rows()) {
    Error err;
    err << __FILE__ << " @" << __LINE__
        << "Dimension mismatch";
  }
  _plan = fftw_plan_dft_1d(
        in.rows(),
        reinterpret_cast<fftw_complex *>(in.data()),
        reinterpret_cast<fftw_complex *>(out.data()),
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT::FFT(CVector &inout, Direction dir)
{
  _plan = fftw_plan_dft_1d(
        inout.rows(),
        reinterpret_cast<fftw_complex *>(inout.data()),
        reinterpret_cast<fftw_complex *>(inout.data()),
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT::FFT(CMatrix &in, CMatrix &out, Direction dir) {
  if (in.rows() != out.rows()) {
    Error err;
    err << __FILE__ << " @" << __LINE__
        << "Dimension mismatch";
  }
  if (in.cols() != out.cols()) {
    Error err;
    err << __FILE__ << " @" << __LINE__
        << "Dimension mismatch";
  }

  int iRowStride = in.rowStride(),  iColStride = in.colStride();
  int oRowStride = out.rowStride(), oColStride = out.colStride();
  int n[1] = { in.rows() };
  _plan = fftw_plan_many_dft(
        1, n, in.cols(),
        reinterpret_cast<fftw_complex *>(in.data()), 0, iRowStride, iColStride,
        reinterpret_cast<fftw_complex *>(out.data()), 0, oRowStride, oColStride,
        (FORWARD == dir) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT::FFT(CMatrix &inout, Direction dir) {
  int n[1] = { inout.rows() };
  int rowStride = inout.rowStride(), colStride = inout.colStride();
  _plan = fftw_plan_many_dft(
        1, n, inout.cols(),
        reinterpret_cast<fftw_complex *>(inout.data()), 0, rowStride, colStride,
        reinterpret_cast<fftw_complex *>(inout.data()), 0, rowStride, colStride,
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT::~FFT() {
  fftw_destroy_plan(_plan);
}

void
FFT::exec() {
  fftw_execute(_plan);
}

void
FFT::exec(CVector &in, CVector &out, Direction dir) {
  FFT fft(in, out, dir);
  fft.exec();
}

void
FFT::exec(CMatrix &in, CMatrix &out, Direction dir) {
  FFT fft(in, out, dir);
  fft.exec();
}

void
FFT::exec(CVector &inout, Direction dir) {
  FFT fft(inout, dir);
  fft.exec();
}

void
FFT::exec(CMatrix &inout, Direction dir) {
  FFT fft(inout, dir);
  fft.exec();
}

size_t
FFT::roundUp(size_t N) {
  if (0 == N) { return 0; }
  return std::pow(2, std::ceil(std::log2(N)));
}
