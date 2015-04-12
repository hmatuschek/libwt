#include "wavelet.hh"

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
 * Implementation of Wavelet container
 * ******************************************************************************************** */
Wavelet::Wavelet()
  : Container(), _wavelet(0)
{
  // pass...
}

Wavelet::Wavelet(WaveletObj *obj)
  : Container(obj), _wavelet(obj)
{
  // pass...
}

Wavelet::Wavelet(const Wavelet &other)
  : Container(other), _wavelet(other._wavelet)
{
  // pass...
}

Wavelet::~Wavelet() {
  // pass...
}

Wavelet &
Wavelet::operator =(const Wavelet &other) {
  Container::operator =(other);
  _wavelet = other._wavelet;
  return *this;
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

double
MorletObj::timeWidth() const {
  // 99% power at scale 1
  return 6./sqrt(_dff);
}

double
MorletObj::specWidth() const {
  // 99% power at scale 1
  return 6*sqrt(_dff);
}


/* ******************************************************************************************** *
 * Implementation of Morlet container
 * ******************************************************************************************** */
Morlet::Morlet(double dff)
  : Wavelet(new MorletObj(dff)), _morlet(static_cast<MorletObj *>(_wavelet))
{
  // pass...
}

Morlet::Morlet(MorletObj *obj)
  : Wavelet(obj), _morlet(obj)
{
  // pass...
}

Morlet::Morlet(const Morlet &other)
  : Wavelet(other), _morlet(other._morlet)
{
  // pass...
}

Morlet::~Morlet() {
  // pass...
}

Morlet &
Morlet::operator =(const Morlet &other) {
  Wavelet::operator =(other);
  _morlet = other._morlet;
  return *this;
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

Scalar
CauchyObj::timeWidth() const {
  double eps = 1e-3;
  return ( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}

Scalar
CauchyObj::specWidth() const {
  double eps = 1e-3;
  return 1./( _alpha*_alpha * ( std::pow(eps, -2. / (_alpha+1)) - 1) / ((2*M_PI)*(2*M_PI)) );
}


Cauchy::Cauchy(double alpha)
  : Wavelet(new CauchyObj(alpha)), _cauchy(static_cast<CauchyObj *>(_wavelet))
{
  // pass...
}

Cauchy::Cauchy(CauchyObj *obj)
  : Wavelet(obj), _cauchy(obj)
{
  // pass...
}

Cauchy::Cauchy(const Cauchy &other)
  : Wavelet(other), _cauchy(other._cauchy)
{
  // pass...
}

Cauchy::~Cauchy() {
  // pass...
}

Cauchy &
Cauchy::operator=(const Cauchy &other) {
  Wavelet::operator =(other);
  _cauchy = other._cauchy;
  return *this;
}


