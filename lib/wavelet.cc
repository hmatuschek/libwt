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

double
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

std::complex<double>
MorletObj::evalAnalysis(const double &t) const {
  return std::exp(std::complex<double>(-t*t*_dff, 2*M_PI*t)) * std::sqrt(_dff/(2*M_PI));
}

std::complex<double>
MorletObj::evalSynthesis(const double &t) const {
  return this->evalAnalysis(t);
}

std::complex<double>
MorletObj::evalRepKern(const double &b, const double &a) const {

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
  // Compute normalization constant, a.k.a. c_gh
  _norm = std::exp( -2*std::log(2*M_PI) - std::lgamma(2*_alpha+1)/2 + std::lgamma(_alpha+1)
                    + (2*_alpha+1)*std::log(2)/2 + std::log(_alpha));
}

CauchyObj::~CauchyObj() {
  // pass...
}

std::complex<double>
CauchyObj::evalAnalysis(const double &t) const {
  return std::pow(std::complex<double>(1, -2*M_PI*t/_alpha), -1-_alpha);
}

std::complex<double>
CauchyObj::evalSynthesis(const double &t) const {
  return CauchyObj::evalAnalysis(t);
}

std::complex<double>
CauchyObj::evalRepKern(const double &b, const double &a) const {
  return std::tgamma(2*_alpha+1) * std::pow(a, _alpha) *
      std::pow(std::complex<double>(1+a, -2*M_PI*b/_alpha), -(1+2*_alpha))/(2*M_PI);
}

double
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
