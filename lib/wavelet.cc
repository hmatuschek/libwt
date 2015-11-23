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

WaveletObj::Scalar
WaveletObj::normConstant() const {
  return 1.0;
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

WaveletObj::CScalar
MorletObj::evalAnalysis(const Scalar &t) const {
  return std::exp(CScalar(-t*t*_dff, 2*M_PI*t))*std::sqrt(_dff/(2*M_PI));
}

WaveletObj::CScalar
MorletObj::evalSynthesis(const Scalar &t) const {
  return this->evalAnalysis(t);
}

WaveletObj::CScalar
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
  // 99.% power at scale 1
  return 1+3.*std::sqrt(_dff);
}


/* ******************************************************************************************** *
 * Implementation of Couchy wavelet object
 * ******************************************************************************************** */
CauchyObj::CauchyObj(double alpha)
  : WaveletObj(), _alpha(alpha)
{
  // Compute normalization constant
  _norm = std::exp( -2*std::log(2*M_PI) - std::lgamma(2*_alpha+1)/2 + std::lgamma(_alpha+1)
                    + (2*_alpha+1)*std::log(2)/2 + std::log(_alpha));
}

CauchyObj::~CauchyObj() {
  // pass...
}

WaveletObj::CScalar
CauchyObj::evalAnalysis(const Scalar &t) const {
  return std::pow(CScalar(1, -2*M_PI*t/_alpha), -1-_alpha);
}

WaveletObj::CScalar
CauchyObj::evalSynthesis(const Scalar &t) const {
  return CauchyObj::evalAnalysis(t);
}

WaveletObj::CScalar
CauchyObj::evalRepKern(const Scalar &b, const Scalar &a) const {
  return std::tgamma(2*_alpha+1) * std::pow(a, _alpha) *
      std::pow(CScalar(1+a, -b/a),-(1+2*_alpha))/(2*M_PI);
}

WaveletObj::Scalar
CauchyObj::normConstant() const {
  return _norm;
}

double
CauchyObj::cutOffTime() const {
  // where the envelope reduced to 1% of max.
  double eps = 1e-2;
  return _alpha*std::sqrt(std::pow(eps, -2/(_alpha+1))-1)/(2*M_PI);
}

double
CauchyObj::cutOffFreq() const {
  double eps = 1e-2;
  return 1+1./( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}
