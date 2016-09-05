#include "polygon.hh"

/* ********************************************************************************************* *
 * Implementation of Polygon
 * ********************************************************************************************* */
Polygon::Polygon()
  : _points(2,0)
{
  // pass...
}

Polygon::Polygon(const Polygon &other)
  : _points(other._points)
{
  // pass...
}

Polygon &
Polygon::operator =(const Polygon &other) {
  _points = other._points;
  return *this;
}

void
Polygon::clear() {
  _points.resize(2,0);
}

size_t
Polygon::size() const {
  return _points.cols();
}

void
Polygon::add(double x, double y) {
  _points.conservativeResize(2, _points.cols()+1);
  _points(0, size()-1) = x;
  _points(1, size()-1) = y;
}

Eigen::Ref<const Eigen::Vector2d>
Polygon::operator ()(size_t i) const {
  return _points.col(i);
}

const Eigen::Matrix<double, 2, Eigen::Dynamic> &
Polygon::points() const {
  return _points;
}

bool
Polygon::isClosed() const {
  if (2 > _points.cols())
    return false;
  return _points.col(0) == _points.col(_points.cols()-1);
}

bool
Polygon::inside(double x, double y) const {
  Eigen::Vector2d q; q << x, y;
  return inside(q);
}

bool
Polygon::inside(const Eigen::Ref<const Eigen::Vector2d> &q) const {
  int t = -1;
  for (int i=1; i<_points.cols(); i++) {
    t *= crossProdTest(q, _points.col(i-1), _points.col(i));
  }
  return t>=0;
}

int
Polygon::crossProdTest(Eigen::Vector2d a, Eigen::Vector2d b, Eigen::Vector2d c) const
{
  if ((a(1)==b(1)) && (b(1)==c(1))) {
    if ( ((b(0) <= a(0)) && (a(0) <= c(0))) || ((c(0)<=a(0)) && (a(0)<=b(0))) )
      return 0;
    else
      return 1;
  }
  if (b(1) > c(1))
    b.swap(c);
  if ((a(1) == b(1)) && (a(0) == b(0)))
    return 0;
  if ((a(1) <= b(1)) || (a(1)>c(1)))
    return 1;
  double d = ( (b(0)-a(0))*(c(1)-a(1)) -
               (b(1)-a(1))*(c(0)-a(0)) );
  if (d > 0)
    return -1;
  if (d < 0)
    return 1;
  return 0;
}


