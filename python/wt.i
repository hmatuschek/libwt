%module pywt
%include "std_complex.i"

%{
#include "wt.hh"
using namespace wt;
#define SWIG_FILE_WITH_INIT
%}

%include "numpy.i"

%init %{
import_array();
%}

class Wavelet {
public:
  virtual ~Wavelet();
  std::complex<double> eval(double t);
  std::complex<double> operator() (double t);
};

class Morlet: public Wavelet {
public:
  Morlet(double dff=2.0);
  virtual ~Morlet();
};

%numpy_typemaps(std::complex<double> , NPY_CDOUBLE, int)
%apply (double* IN_ARRAY1, int DIM1) {(double* scales, int Nscales)};
%apply (std::complex<double>* IN_ARRAY1, int DIM1) {(std::complex<double>* signal, int Nsig)};
%apply (std::complex<double>* INPLACE_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* out, int Nrow, int Ncol)};

class WaveletTransform {
public:
  WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales);
  virtual ~WaveletTransform();
};

%extend WaveletTransform {
void operator() (std::complex<double> *signal, int Nsig, std::complex<double> *out, int Nrow, int Ncol) {
  if (Nsig != Nrow) {
    PyErr_Format(PyExc_ValueError,
                 "Signal length and output rows do not match!");
    return;
  }
  if (Ncol != self->nScales()) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and output columns do not match!");
    return;
  }
  Eigen::Map<CVector> signalMap(signal, Nsig);
  Eigen::Map<CMatrix> outMap(out, Nsig, Ncol);
  (*self)(signalMap, outMap);
}
}

