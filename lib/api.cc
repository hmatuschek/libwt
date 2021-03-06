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

double
Morlet::dff() const {
  return _morlet->dff();
}


/* ******************************************************************************************** *
 * Implementation of RegMorlet container
 * ******************************************************************************************** */
RegMorlet::RegMorlet(double dff)
  : Wavelet(new RegMorletObj(dff)), _morlet(static_cast<RegMorletObj *>(_wavelet))
{
  // pass...
}

RegMorlet::RegMorlet(RegMorletObj *obj)
  : Wavelet(obj), _morlet(obj)
{
  // pass...
}

RegMorlet::RegMorlet(const RegMorlet &other)
  : Wavelet(other), _morlet(other._morlet)
{
  // pass...
}

RegMorlet::~RegMorlet() {
  // pass...
}

RegMorlet &
RegMorlet::operator =(const RegMorlet &other) {
  Wavelet::operator =(other);
  _morlet = other._morlet;
  return *this;
}

double
RegMorlet::dff() const {
  return _morlet->dff();
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

double
Cauchy::alpha() const {
  return _cauchy->alpha();
}


/* ******************************************************************************************** *
 * Implementation of RegCauchy container
 * ******************************************************************************************** */
RegCauchy::RegCauchy(double alpha)
  : Wavelet(new RegCauchyObj(alpha)), _cauchy(static_cast<RegCauchyObj *>(_wavelet))
{
  // pass...
}

RegCauchy::RegCauchy(RegCauchyObj *obj)
  : Wavelet(obj), _cauchy(obj)
{
  // pass...
}

RegCauchy::RegCauchy(const RegCauchy &other)
  : Wavelet(other), _cauchy(other._cauchy)
{
  // pass...
}

RegCauchy::~RegCauchy() {
  // pass...
}

RegCauchy &
RegCauchy::operator=(const RegCauchy &other) {
  Wavelet::operator =(other);
  _cauchy = other._cauchy;
  return *this;
}

double
RegCauchy::alpha() const {
  return _cauchy->alpha();
}


/* ******************************************************************************************** *
 * Implementation of linear, dyadic etc.
 * ******************************************************************************************** */
void
wt::linear_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out) {
  size_t N = out.size();
  double dx = (b-a)/N;
  for (size_t i=0; i<N; i++, a+=dx) {
    out(i) = a;
  }
}

void
wt::dyadic_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out) {
  a = std::log2(a); b = std::log2(b);
  size_t N = out.size();
  double dx = (b-a)/N;
  for (size_t i=0; i<N; i++, a+=dx) {
    out(i) = std::pow(2, a);
  }
}

void
wt::decadic_range(double a, double b, Eigen::Ref<Eigen::VectorXd> out) {
  a = std::log10(a); b = std::log10(b);
  size_t N = out.size();
  double dx = (b-a)/N;
  for (size_t i=0; i<N; i++, a+=dx) {
    out(i) = std::pow(10, a);
  }
}
