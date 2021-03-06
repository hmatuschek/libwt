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

double
MorletObj::dff() const {
  return _dff;
}

std::complex<double>
MorletObj::evalAnalysis(const double &t) const {
  return std::exp(std::complex<double>(-t*t*_dff/2, 2*M_PI*t)) * std::sqrt(_dff/(2*M_PI));
}

std::complex<double>
MorletObj::evalSynthesis(const double &t) const {
  return this->evalAnalysis(t);
}

std::complex<double>
MorletObj::evalRepKern(const double &b, const double &a) const {
  double a2p1 = (a*a+1.), am1 = (a-1.);
  double d = _dff, d2 = d*d;
  double c = d2/std::sqrt(2*M_PI*d*a2p1);
  double re = -(d2*b*b + 4*M_PI*M_PI*am1*am1)/(2*d*a2p1);
  double im = 2*M_PI*b*(1. + am1/a2p1)/a;
  return  c * std::exp(std::complex<double>(re, im));
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
 * Implementation of RegMorletObj wavelet
 * ******************************************************************************************** */
RegMorletObj::RegMorletObj(double dff)
  : WaveletObj(), _dff(dff)
{
  // pass...
}

RegMorletObj::~RegMorletObj() {
  // pass...
}

double
RegMorletObj::dff() const {
  return _dff;
}

std::complex<double>
RegMorletObj::evalAnalysis(const double &t) const {
  return std::exp(std::complex<double>(-t*t*_dff/2, -2*M_PI*t)) * std::sqrt(_dff/(2*M_PI));
}

std::complex<double>
RegMorletObj::evalSynthesis(const double &t) const {
  return this->evalAnalysis(t);
}

std::complex<double>
RegMorletObj::evalRepKern(const double &b, const double &a) const {
  double a2p1 = (a*a+1.), am1 = (a-1.);
  double d = _dff, d2 = d*d;
  double c = d2/std::sqrt(2*M_PI*d*a2p1);
  double re = -(d2*b*b + 4*M_PI*M_PI*am1*am1)/(2*d*a2p1);
  double im = 2*M_PI*b*(1. + am1/a2p1)/a;
  return  c * std::exp(std::complex<double>(re, im));
}

double
RegMorletObj::cutOffTime() const {
  // 99% power at scale 1
  return 3./std::sqrt(_dff);
}

double
RegMorletObj::cutOffFreq() const {
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

double
CauchyObj::alpha() const {
  return _alpha;
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
  double c = _alpha*std::log(a) + std::lgamma(2*_alpha-1) - (1+2*_alpha)*log(2*M_PI);
  return std::exp(c) * std::pow(std::complex<double>(1+a, -2*M_PI*b/_alpha), -(1+2*_alpha));
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


/* ******************************************************************************************** *
 * Implementation of RegCouchy wavelet object
 * ******************************************************************************************** */
RegCauchyObj::RegCauchyObj(double alpha)
  : WaveletObj(), _alpha(alpha)
{
  // Compute normalization constant, a.k.a. c_gh
  _norm = std::exp( -2*std::log(2*M_PI) - std::lgamma(2*_alpha+1)/2 + std::lgamma(_alpha+1)
                    + (2*_alpha+1)*std::log(2)/2 + std::log(_alpha));
}

RegCauchyObj::~RegCauchyObj() {
  // pass...
}

double
RegCauchyObj::alpha() const {
  return _alpha;
}

std::complex<double>
RegCauchyObj::evalAnalysis(const double &t) const {
  return std::pow(std::complex<double>(1, 2*M_PI*t/_alpha), -1-_alpha);
}

std::complex<double>
RegCauchyObj::evalSynthesis(const double &t) const {
  return RegCauchyObj::evalAnalysis(t);
}

std::complex<double>
RegCauchyObj::evalRepKern(const double &b, const double &a) const {
  double c = _alpha*std::log(a) + std::lgamma(2*_alpha-1) - (1+2*_alpha)*log(2*M_PI);
  return std::exp(c) * std::pow(std::complex<double>(1+a, 2*M_PI*b/_alpha), -(1+2*_alpha));
}

double
RegCauchyObj::normConstant() const {
  return _norm;
}

double
RegCauchyObj::cutOffTime() const {
  // where the envelope reduced to 1% of max.
  double eps = 1e-2;
  return _alpha*std::sqrt(std::pow(eps, -2/(_alpha+1))-1)/(2*M_PI);
}

double
RegCauchyObj::cutOffFreq() const {
  double eps = 1e-2;
  return 1+1./( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}
