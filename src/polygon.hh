#ifndef POLYGON_HH
#define POLYGON_HH

#include <Eigen/Eigen>

/** Represents a polygon.
 * This class allows for testing point laying in the interior of the polygon. */
class Polygon
{
public:
  /** Empty constructor. */
  Polygon();
  /** Copy constructor. */
  Polygon(const Polygon &other);
  /** Assingment operator. */
  Polygon &operator=(const Polygon &other);

  /** Clears the polygon. */
  void clear();
  /** Returns the number of points of the polygon. */
  size_t size() const;
  /** Adds a point to the polygon. */
  void add(double x, double y);
  /** Returns the i-th point of the polygon. */
  Eigen::Ref<const Eigen::Vector2d> operator() (size_t i) const;
  /** Returns all points of the polygon. */
  const Eigen::Matrix<double, 2, Eigen::Dynamic> &points() const;

  /** Returns @c true if the polygon is closed. That is, if the last point is identical to
   * the first. */
  bool isClosed() const;
  /** Returns @c true if the given point lays within or on the polygon. */
  bool inside(double x, double y) const;
  /** Returns @c true if the given point lays within or on the polygon. */
  bool inside(const Eigen::Ref<const Eigen::Vector2d> &q) const;

protected:
  /** Determines whether the ray from a to the right crosses a-b. */
  int crossProdTest(Eigen::Vector2d a,
                    Eigen::Vector2d b,
                    Eigen::Vector2d c) const;

protected:
  /** The polygon points. */
  Eigen::Matrix<double, 2, Eigen::Dynamic> _points;
};


#endif // POLYGON_HH
