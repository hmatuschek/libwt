#include "api.hh"

using namespace wt;

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
 * Implementation of Cauchy container
 * ******************************************************************************************** */
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


