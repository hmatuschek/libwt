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

class Cauchy: public Wavelet {
public:
  Cauchy(double alpha=1.0);
  virtual ~Cauchy();
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


%apply (std::complex<double>* IN_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* transformed, int Nrow, int Ncol)};
%apply (std::complex<double>* INPLACE_ARRAY1, int DIM1) {(std::complex<double>* out, int N)};

class WaveletSynthesis {
public:
  WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales);
  virtual ~WaveletSynthesis();
};

%extend WaveletSynthesis {
void operator() (std::complex<double>* transformed, int Nrow, int Ncol, std::complex<double>* out, int N) {
  if (N != Nrow) {
    PyErr_Format(PyExc_ValueError,
                 "Transformed rows and output length do not match!");
    return;
  }
  if (Ncol != self->nScales()) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and transformed columns do not match!");
    return;
  }
  Eigen::Map<CMatrix> transformedMap(transformed, N, Ncol);
  Eigen::Map<CVector> outMap(out, N);
  (*self)(transformedMap, outMap);
}
}
