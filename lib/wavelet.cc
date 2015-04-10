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
MorletObj::eval(const Scalar &t) const {
  return std::exp(CScalar(-t*t*_dff/2.0, 2*M_PI*t))*std::sqrt(_dff/(2*M_PI));
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
  // pass...
}

CauchyObj::~CauchyObj() {
  // pass...
}

CScalar
CauchyObj::eval(const Scalar &t) const {
  return std::pow(CScalar(1,-2*M_PI*t/_alpha), -1-_alpha);
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


