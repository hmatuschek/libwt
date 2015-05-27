#ifndef __WT_MOVINGAVERAGE_HH__
#define __WT_MOVINGAVERAGE_HH__

#include <Eigen/Eigen>

namespace wt {

/** A simple vector-valued moving sum. */
template <class Scalar>
class MovingSum
{
public:
  /** Specifies the vector type to sum over. */
  typedef Eigen::Matrix<Scalar, 1, Eigen::Dynamic> RowVector;

public:
  /** Constructor.
   * @param N Specifies the number of vectors to sum over.
   * @param M Specifies the dimension of the vectors. */
  MovingSum(size_t N, size_t M)
    : _hist(N,M), _current(N), _currentIdx(0)
  {
    _hist.setZero();
    _current.setZero();
  }

  /** Resizes the moving sum operator.
   * @param N Specifies the number of vector to sum over.
   * @param M Specifies the dimension of the vectors. */
  void resize(size_t N, size_t M) {
    _hist.resize(N,M);  _hist.setZero();
    _current.resize(N); _current.setZero();
    _currentIdx=0;
  }

  /** Adds a vector and updates the moving sum. */
  template <class Derived>
  const RowVector &operator() (const Eigen::DenseBase<Derived> &in) {
    _current -= _hist.row(_currentIdx);
    _hist.row(_currentIdx) = in;
    _current += _hist.row(_currentIdx);
    _currentIdx++;
    // modulo N
    if (_hist.rows()==_currentIdx) { _currentIdx=0; }
    return _current;
  }

  /** Adds a vector and updates the moving sum. */
  const RowVector &addZero() {
    _current -= _hist.row(_currentIdx);
    _hist.row(_currentIdx).setZero();
    _currentIdx++;
    // modulo N
    if (_hist.rows()==_currentIdx) { _currentIdx=0; }
    return _current;
  }

protected:
  /** Holds the "history" of the last @c N vectors (stored in rows). */
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> _hist;
  /** Holds the current sum. */
  Eigen::Matrix<Scalar, 1, Eigen::Dynamic> _current;
  /** Holds the index, where the next vector should be inserted into
   * @c _hist. */
  size_t _currentIdx;
};

}

#endif // __WT_MOVINGAVERAGE_HH__
