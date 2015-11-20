#ifndef __WT_CONVOLUTION_HH__
#define __WT_CONVOLUTION_HH__

#include "types.hh"
#include "fft.hh"

namespace wt {

/** Implements the overlap-add covolution\cite Smith2012 of a signal with several filter kernels
 * of the same size, each being shorter that the signal.
 *
 * As all kernels share the same size, the forward FFT of a piece of the input signal must
 * be computed only once. This speeds up the convolution slightly. Further, assuming K kernels of
 * size M and a signal length of N samples. The direct FFT convolution cost would be
 * \f$(K+1)\,N\,\log(N)\f$, not including the costs of computing the FFTs of the kernels.
 *
 * Using the overlap-add method, the costs are \f$(K+1)\,N\,\log(2\,M)\f$, which results into a
 * benifit if \f$2\,M < N\f$. */
template <typename Scalar>
class Convolution
{
public:
  typedef typename Traits<Scalar>::Complex Complex;
  typedef typename Traits<Scalar>::CVector CVector;
  typedef typename Traits<Scalar>::CMatrix CMatrix;

public:
  /** Constructor. The complex matrix @c kernels specifies the convolution filters to be used.
   * Every colum specifies a filter kernel. */
  Convolution(const CMatrix &kernels, size_t subSample = 1)
    : _K(kernels.cols()), _M(kernels.rows()),
      _kernelF(2*_M, _K), _part(2*_M), _fwd(_part, FFT<Scalar>::FORWARD),
      _lastRes(_M, _K), _work(2*_M, _K), _rev(_work, FFT<Scalar>::BACKWARD),
      _subSampling(subSample)
  {
    // Store filter kernels:
    this->_kernelF.topRows(this->_M).noalias() = kernels;
    this->_kernelF.bottomRows(this->_M).setConstant(0);
    // Compute FFT in-place
    FFT<Scalar>::exec(this->_kernelF, FFT<Scalar>::FORWARD);
  }

  Convolution(const Complex *kernels, int Nrow, int Ncol, size_t subSample=1)
    : _K(Ncol), _M(Nrow),
      _kernelF(2*_M, _K), _part(2*_M), _fwd(_part, FFT<Scalar>::FORWARD),
      _lastRes(_M, _K), _work(2*_M, _K), _rev(_work, FFT<Scalar>::BACKWARD),
      _subSampling(subSample)
  {
    // Store filter kernels:
    _kernelF.topRows(_M).noalias() = Eigen::Map<const CMatrix>(kernels, _M, _K);
    _kernelF.bottomRows(_M).setConstant(0);
    // Compute FFT in-place
    FFT<Scalar>::exec(_kernelF, FFT<Scalar>::FORWARD);
  }

  /** Performs the convolution of the signal passed by @c signal with the kernels passed to the
   * constructor. The results are stored in the columns of the array @c out. Hence, given a
   * signal with N samples and K kernels, the output must be pre-allocated as a NxK array/matrix. */
  template <class iDerived, class oDerived>
  void apply(const Eigen::DenseBase<iDerived> &signal, Eigen::DenseBase<oDerived> &out)
  {
    // First, clear _lastRes matrix
    this->_lastRes.setConstant(0);

    /*
     * Perform overlap-add FFT
     */
    size_t N = signal.size();
    size_t steps = N/this->_M;
    size_t rem   = N%this->_M;
    size_t out_offset = 0;

    // Compute the first complete steps
    for (size_t i=0; i<steps; i++) {
      // Store piece into forward-trafo buffer
      this->_part.head(this->_M).noalias() =
          signal.block(i*this->_M,0, _M,1).template cast<Complex>();
      // 0-pad
      this->_part.tail(this->_M).setConstant(0);

      // perform forward FFT
      this->_fwd.exec();

      // Multiply result of forward FFT of the signal piece with every (transformed) kernel
      for (size_t j=0; j<this->_K; j++) {
        this->_work.col(j).noalias() =
            this->_part.cwiseProduct(this->_kernelF.col(j));
      }

      // Peform backward trafo
      this->_rev.exec();

      /*
       * Compute result of convolution and store it into the output buffer
       */
      if (0 == out_offset) { // first block
        out.topRows(this->_M/2).noalias() =
            ( ( this->_work.block(this->_M/2, 0, this->_M/2, this->_K)
                + this->_lastRes.topRows(this->_M/2) )/(2*this->_M) );
        out_offset += ( this->_M/2 );
      } else { // intermediate blocks
        out.block(out_offset, 0, this->_M, this->_K).noalias() =
            ( ( this->_work.topRows(this->_M) + this->_lastRes)/(2*this->_M) );
        out_offset += this->_M;
      }
      // Store remaining part for next step unless we are at the last step
      this->_lastRes.noalias() = this->_work.bottomRows(this->_M);
    }

    // Perform final step (if rem>0)
    if (rem) {
      // Store remaining samples
      this->_part.head(rem).noalias() = signal.block(steps*this->_M,0, rem,1);
      // 0-pad
      this->_part.tail(2*this->_M-rem).setConstant(0);

      // perform forward FFT
      this->_fwd.exec();

      // Multiply result of forward FFT of the signal piece with every (transformed) kernel
      for (size_t j=0; j<this->_K; j++) {
        this->_work.col(j).noalias() =
            this->_part.cwiseProduct(this->_kernelF.col(j));
      }

      // Peform backward trafo
      this->_rev.exec();

      /// @bug Does not work if signal is shorter than kernel!!!
      if (0 == steps) {
        out.block(0, 0, rem, this->_K).noalias() =
            this->_work.block(this->_M/2, 0, rem, this->_K) / (2*this->_M) ;
      } else if (this->_M >= (rem+this->_M/2)) {
        out.block(out_offset, 0, rem+this->_M/2, this->_K).noalias() =
            ( ( this->_work.topRows(rem+this->_M/2) +
                this->_lastRes.topRows(rem+this->_M/2) ) / (2*this->_M) );
      } else {
        out.block(out_offset, 0, this->_M, this->_K).noalias() =
            ( ( this->_work.topRows(this->_M) +
                this->_lastRes.topRows(this->_M) ) / (2*this->_M) );
        out_offset += this->_M;
        size_t n = rem+this->_M/2-this->_M;
        out.block(out_offset, 0, n, this->_K).noalias() =
            (this->_work.block(this->_M, 0, n, this->_K) / (2*this->_M));
      }
    } else {
      // store last _M/2 samples if rem==0
      out.block(out_offset, 0, this->_M/2, this->_K).noalias() =
          ( this->_work.block(this->_M-rem, 0, this->_M/2, this->_K) / (2*this->_M) );
    }
  }

  /** Returns the length of the kernels. */
  inline size_t kernelLength() const { return this->_M; }
  /** Returns the number of kernels. */
  inline size_t numKernels() const { return this->_K; }

  /** Returns the sub-sampling assinged to the convolution operation. */
  inline size_t subSampling() const { return _subSampling; }
  /** Sets the sub-sampling assinged to the convolution operation. */
  void setSubSampling(size_t subSample) { _subSampling = subSample; }

protected:
  /** The number of kernels. */
  size_t _K;
  /** The lenght of the kernels. */
  size_t _M;
  /** Holds the Fourier transformed of the kernels. */
  CMatrix _kernelF;

  /** Working vector for the forward-transform of an piece of the input signal. */
  CVector _part;
  /** The in-place FFT transform of a (zero-padded) signal part. */
  FFT<Scalar> _fwd;

  /** Second halfs of the back-transformed, filtered singals. */
  CMatrix _lastRes;
  /** Working memory for backward transformation of filtered signals. */
  CMatrix _work;
  /** Backward transformation. */
  FFT<Scalar> _rev;

  /** Possible subsampling for the convolution kernels.
   * This property of the convolution is not computed nor handled by the convolution itself,
   * instead it is a property that can be assigned to it. I.e., a kind of meta-data
   * for the convolution operation. */
  size_t _subSampling;
};

}

#endif // __WT_CONVOLUTION_HH__
