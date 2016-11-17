%module wt
%include "std_complex.i"

%{
#include "wt.hh"
#define SWIG_FILE_WITH_INIT
%}

%include "numpy.i"

%init %{
import_array();
%}

namespace wt {
/*
 * Interfacing Wavelet classes
 */
%feature("autodoc", "Base class of all mother wavelets.");
class Wavelet
{
protected:
  %feature("autodoc", "Hidden constructor.");
  Wavelet();

public:
  virtual ~Wavelet();
  %feature("autodoc", "Evaluates the unscaled analysis mother wavelet.");
  std::complex<double> evalAnalysis(double t);
  %feature("autodoc", "Evaluates the unscaled synthesis mother wavelet.");
  std::complex<double> evalSynthesis(double t);
  %feature("autodoc", "Evaluates the reproducing kernel located at scale 1 and time 0.");
  std::complex<double> evalRepKern(double b, double a);
  %feature("autodoc", "Retruns the normalization constant.");
  double normConstant() const;
  %feature("autodoc", "Returns the width of the unscaled (mother) wavelet in the time domain.");
  double cutOffTime() const;
  %feature("autodoc", "Returns the width of the unscaled (mother) wavelet in the frequency domain.");
  double cutOffFreq() const;
};


%feature("autodoc","Implements the Morlet \"wavelet\".");
class Morlet: public Wavelet
{
public:
  %feature("autodoc", "Constructor. The argument dff specifies the frequency resolution.");
  Morlet(double dff=2.0);
  virtual ~Morlet();
};


%feature("autodoc","Implements the Cauchy or Paul wavelet.");
class Cauchy: public Wavelet
{
public:
  %feature("autodoc", "Constructor. The argument alpha specifies the time resolution.");
  Cauchy(double alpha=1.0);
  virtual ~Cauchy();
};

}


/*
 * Interfacing Convolution class
 */
%numpy_typemaps(std::complex<double> , NPY_CDOUBLE, int)
%apply (std::complex<double>* IN_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* kernels, int Nrow, int Ncol)};
%apply (std::complex<double>* IN_ARRAY1, int DIM1) {(std::complex<double>* signal, int Nsig)};
%apply (std::complex<double>* INPLACE_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* out, int Nrow, int Ncol)};

namespace wt {
%feature("autodoc", "Implements overlap-add convolution.");
class Convolution
{
public:
  Convolution(std::complex<double> *kernels, int Nrow, int Ncol);

  size_t kernelLength() const;
  size_t numKernels() const;
};
}

%extend wt::Convolution {
  void apply (std::complex<double> *signal, int Nsig, std::complex<double> *out, int Nrow, int Ncol) {
    if (Nsig != Nrow) {
      PyErr_Format(PyExc_ValueError,
                   "Signal length and output rows do not match!");
      return;
    }
    if (Ncol != int(self->numKernels())) {
      PyErr_Format(PyExc_ValueError,
                   "Number of kernels and output columns do not match!");
      return;
    }
    Eigen::Map<Eigen::VectorXcd> signalMap(signal, Nsig);
    Eigen::Map<Eigen::MatrixXcd> outMap(out, Nsig, Ncol);
    self->apply(signalMap, outMap);
  }
}


/*
 * Interfacing WaveletAnalysis class.
 */
%apply (double* OUT_ARRAY1, int DIM1) {(double* outScales, int Nscales)};
%apply (double* IN_ARRAY1, int DIM1) {(double* scales, int Nscales)};


namespace wt {
%feature("autodoc", "Base class of all wavelet analyses.");
class WaveletAnalysis
{
protected:
  WaveletAnalysis(const Wavelet &wavelet, double *scales, int Nscales);
public:
  size_t nScales() const;
  void scales(double *outScales, int Nscales) const;
  const Wavelet &wavelet() const;
};
}

/*
 * Interfacing WaveletTransform class
 */
%numpy_typemaps(std::complex<double> , NPY_CDOUBLE, int)
%apply (double* IN_ARRAY1, int DIM1) {(double* scales, int Nscales)};
%apply (std::complex<double>* IN_ARRAY1, int DIM1) {(std::complex<double>* signal, int Nsig)};
%apply (std::complex<double>* INPLACE_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* out, int Nrow, int Ncol)};

namespace wt {
%feature("autodoc", "Implements the continous wavelet transform.");
class WaveletTransform: public WaveletAnalysis
{
public:
  WaveletTransform(const Wavelet &wavelet, double *scales, int Nscales, bool subSample=false);
  virtual ~WaveletTransform();
};
}

%extend wt::WaveletTransform {
void operator() (std::complex<double> *signal, int Nsig, std::complex<double> *out, int Nrow, int Ncol) {
  if (Nsig != Nrow) {
    PyErr_Format(PyExc_ValueError,
                 "Signal length and output rows do not match!");
    return;
  }
  if (Ncol != int(self->nScales())) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and output columns do not match!");
    return;
  }
  Eigen::Map<Eigen::VectorXcd> signalMap(signal, Nsig);
  Eigen::Map<Eigen::MatrixXcd> outMap(out, Nsig, Ncol);
  (*self)(signalMap, outMap);
}
}


/*
 * Interfacing WaveletSynthesis class
 */
%apply (std::complex<double>* IN_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* transformed, int Nrow, int Ncol)};
%apply (std::complex<double>* INPLACE_ARRAY1, int DIM1) {(std::complex<double>* out, int N)};

namespace wt {
%feature("autodoc", "Implements the wavelet synthesis (inverse transform).");
class WaveletSynthesis: public WaveletAnalysis
{
public:
  WaveletSynthesis(const Wavelet &wavelet, double *scales, int Nscales);
  WaveletSynthesis(const WaveletAnalysis &wt);
  virtual ~WaveletSynthesis();
};
}

%extend wt::WaveletSynthesis {
void operator() (std::complex<double>* transformed, int Nrow, int Ncol, std::complex<double>* out, int N) {
  if (N != Nrow) {
    PyErr_Format(PyExc_ValueError,
                 "Transformed rows and output length do not match!");
    return;
  }
  if (Ncol != int(self->nScales())) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and transformed columns do not match!");
    return;
  }
  Eigen::Map<Eigen::MatrixXcd> transformedMap(transformed, N, Ncol);
  Eigen::Map<Eigen::VectorXcd> outMap(out, N);
  (*self)(transformedMap, outMap);
}
}


/*
 * Interfacing WaveletConvolution class
 */
%apply (std::complex<double>* IN_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* transformed, int Nrow, int Ncol)};
%apply (std::complex<double>* INPLACE_FARRAY2, int DIM1, int DIM2) {(std::complex<double>* out, int Mrow, int Mcol )};

namespace wt {
%feature("autodoc", "Implements the convolution of a complex function in the time-scale plane with the reproducing kernel.");
class WaveletConvolution: public WaveletAnalysis
{
public:
  WaveletConvolution(const Wavelet &wavelet, double *scales, int Nscales);
  WaveletConvolution(const WaveletAnalysis &wt);
  virtual ~WaveletConvolution();
};
}

%extend wt::WaveletConvolution {
void operator()(std::complex<double>* transformed, int Nrow, int Ncol, std::complex<double>* out, int Mrow, int Mcol) {
  if (Mrow != Nrow) {
    PyErr_Format(PyExc_ValueError,
                 "Transformed rows and output rows do not match!");
    return;
  }
  if (Ncol != int(self->nScales())) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and transformed columns do not match!");
    return;
  }
  if (Mcol != int(self->nScales())) {
    PyErr_Format(PyExc_ValueError,
                 "Number of scales and result columns do not match!");
    return;
  }
  Eigen::Map<Eigen::MatrixXcd> transformedMap(transformed, Nrow, Ncol);
  Eigen::Map<Eigen::MatrixXcd> outMap(out, Mrow, Mcol);
  (*self)(transformedMap, outMap);
}
}


%apply (double* INPLACE_ARRAY1, int DIM1) {(double* out, int N)};
%inline %{
namespace wt {
  void linear_range(double a, double b, double* out, int N) {
    Eigen::Map<Eigen::VectorXd> outMap(out, N);
    wt::linear_range(a,b,outMap);
  }

  void dyadic_range(double a, double b, double* out, int N) {
    Eigen::Map<Eigen::VectorXd> outMap(out, N);
    wt::dyadic_range(a,b,outMap);
  }

  void decadic_range(double a, double b, double* out, int N) {
    Eigen::Map<Eigen::VectorXd> outMap(out, N);
    wt::decadic_range(a,b,outMap);
  }
  }
%}
