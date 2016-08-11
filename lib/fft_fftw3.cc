#include "fft_fftw3.hh"
#include <iostream>
#include <cmath>

using namespace wt;


/* ********************************************************************************************** *
 * Implementation of FFT plan using FFTW3
 * ********************************************************************************************** */
FFT<double>::FFT(CVector &in, CVector &out, Direction dir)
{
  assertShapeN(in, out.rows());
  assertShapeN(out, in.rows());
  _plan = fftw_plan_dft_1d(
        in.rows(),
        reinterpret_cast<fftw_complex *>(in.data()),
        reinterpret_cast<fftw_complex *>(out.data()),
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT<double>::FFT(CVector &inout, Direction dir)
{
  _plan = fftw_plan_dft_1d(
        inout.rows(),
        reinterpret_cast<fftw_complex *>(inout.data()),
        reinterpret_cast<fftw_complex *>(inout.data()),
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT<double>::FFT(CMatrix &in, CMatrix &out, Direction dir) {
  assertShapeNM(in, out.rows(), out.cols());

  int iRowStride = in.rowStride(),  iColStride = in.colStride();
  int oRowStride = out.rowStride(), oColStride = out.colStride();
  int n[1] = { int(in.rows()) };
  _plan = fftw_plan_many_dft(
        1, n, in.cols(),
        reinterpret_cast<fftw_complex *>(in.data()), 0, iRowStride, iColStride,
        reinterpret_cast<fftw_complex *>(out.data()), 0, oRowStride, oColStride,
        (FORWARD == dir) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT<double>::FFT(CMatrix &inout, Direction dir) {
  int n[1] = { int(inout.rows()) };
  int rowStride = inout.rowStride(), colStride = inout.colStride();
  _plan = fftw_plan_many_dft(
        1, n, inout.cols(),
        reinterpret_cast<fftw_complex *>(inout.data()), 0, rowStride, colStride,
        reinterpret_cast<fftw_complex *>(inout.data()), 0, rowStride, colStride,
        (dir == FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD, FFTW_ESTIMATE);
}

FFT<double>::~FFT() {
  fftw_destroy_plan(_plan);
}

void
FFT<double>::exec() {
  fftw_execute(_plan);
}

void
FFT<double>::exec(CVector &in, CVector &out, Direction dir) {
  FFT fft(in, out, dir);
  fft.exec();
}

void
FFT<double>::exec(CMatrix &in, CMatrix &out, Direction dir) {
  FFT fft(in, out, dir);
  fft.exec();
}

void
wt::FFT<double>::exec(CVector &inout, Direction dir) {
  FFT fft(inout, dir);
  fft.exec();
}

void
FFT<double>::exec(CMatrix &inout, Direction dir) {
  FFT fft(inout, dir);
  fft.exec();
}

size_t
FFT<double>::roundUp(size_t N) {
  if (0 == N) { return 0; }
  return std::pow(2, std::ceil(std::log2(N)));
}

