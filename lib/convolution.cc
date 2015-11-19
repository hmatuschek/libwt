#include "convolution.hh"

using namespace wt;


Convolution::Convolution(const CMatrix &kernels, size_t subSample)
  : _K(kernels.cols()), _M(kernels.rows()),
    _kernelF(2*_M, _K), _part(2*_M), _fwd(_part, FFT::FORWARD),
    _lastRes(_M, _K), _work(2*_M, _K), _rev(_work, FFT::BACKWARD),
    _subSampling(subSample)
{
  // Store filter kernels:
  _kernelF.topRows(_M).noalias() = kernels;
  _kernelF.bottomRows(_M).setConstant(0);
  // Compute FFT in-place
  FFT::exec(_kernelF, FFT::FORWARD);
}

Convolution::Convolution(const CScalar *kernels, int Nrow, int Ncol, size_t subSample)
  : _K(Ncol), _M(Nrow),
    _kernelF(2*_M, _K), _part(2*_M), _fwd(_part, FFT::FORWARD),
    _lastRes(_M, _K), _work(2*_M, _K), _rev(_work, FFT::BACKWARD),
    _subSampling(subSample)
{
  // Store filter kernels:
  _kernelF.topRows(_M).noalias() = Eigen::Map<const CMatrix>(kernels, _M, _K);
  _kernelF.bottomRows(_M).setConstant(0);
  // Compute FFT in-place
  FFT::exec(_kernelF, FFT::FORWARD);
}

Convolution::~Convolution() {
  // pass...
}
