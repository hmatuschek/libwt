#include "convolution.hh"

using namespace wt;


Convolution::Convolution(const CMatrix &kernels)
  : _K(kernels.cols()), _M(kernels.rows()),
    _kernelF(2*_M, _K), _part(2*_M), _fwd(_part, FFT::FORWARD),
    _lastRes(_M, _K), _work(2*_M, _K), _rev(_work, FFT::BACKWARD)
{
  _kernelF.setZero();
  // Store filter kernels:
  _kernelF.topRows(_M).noalias() = kernels;
  // Compute FFT in-place
  FFT::exec(_kernelF, FFT::FORWARD);
}

Convolution::~Convolution() {
  // pass...
}
