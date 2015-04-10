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
  return 4*sqrt(_dff);
}

double
MorletObj::specWidth() const {
  return 4/sqrt(_dff);
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
