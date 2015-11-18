#include "wavelet.hh"
#include <cmath>

using namespace wt;


/* ******************************************************************************************** *
 * Implementation of WaveletObj
 * ******************************************************************************************** */
WaveletObj::WaveletObj()
  : Object()
{
  // pass...
}

WaveletObj::~WaveletObj() {
  // pass...
}


/* ******************************************************************************************** *
 * Implementation of Morlet wavelet
 * ******************************************************************************************** */
MorletObj::MorletObj(double dff)
  : WaveletObj(), _dff(dff)
{
  // pass...
}

MorletObj::~MorletObj() {
  // pass...
}

CScalar
MorletObj::evalAnalysis(const Scalar &t) const {
  return std::exp(CScalar(-t*t*_dff, 2*M_PI*t))*std::sqrt(_dff/(2*M_PI));
}

CScalar
MorletObj::evalSynthesis(const Scalar &t) const {
  return this->evalAnalysis(t);
}

CScalar
MorletObj::evalRepKern(const Scalar &b, const Scalar &a) const {
  /// @bug implement
  return 0;
}

double
MorletObj::cutOffTime() const {
  // 99% power at scale 1
  return 3./std::sqrt(_dff);
}

double
MorletObj::cutOffFreq() const {
  // 99% power at scale 1
  return 1+3.*std::sqrt(_dff);
}


/* ******************************************************************************************** *
 * Implementation of Couchy wavelet object
 * ******************************************************************************************** */
CauchyObj::CauchyObj(double alpha)
  : WaveletObj(), _alpha(alpha)
{
  // Compute normalization constant
  _norm = _alpha*exp(
        std::log(std::sqrt(M_PI)) + std::lgamma(_alpha+0.5) - std::lgamma(_alpha+1))/(2*M_PI);
}

CauchyObj::~CauchyObj() {
  // pass...
}

CScalar
CauchyObj::evalAnalysis(const Scalar &t) const {
  return std::pow(CScalar(1,-2*M_PI*t/_alpha), -1-_alpha)/_norm;
}

CScalar
CauchyObj::evalSynthesis(const Scalar &t) const {
  return this->evalAnalysis(t);
}

CScalar
CauchyObj::evalRepKern(const Scalar &b, const Scalar &a) const {
  return std::tgamma(2*_alpha+1) * std::pow(a, _alpha) *
      std::pow(CScalar(1+a, -b/a),-(1+2*_alpha))/(2*M_PI);
}

Scalar
CauchyObj::cutOffTime() const {
  double eps = 1e-4;
  return ( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}

Scalar
CauchyObj::cutOffFreq() const {
  double eps = 1e-4;
  return 1+1./( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}
