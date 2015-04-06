#ifndef __WT_CONVOLUTION_HH__
#define __WT_CONVOLUTION_HH__

#include "types.hh"
#include "fft.hh"

namespace wt {

/** Implements the overlap-add covolution\cite{Smith2015} of a signal with several filter kernels
 * of the same size, each being shorter that the signal.
 *
 * As all kernels share the same size, hence the forward FFT of a piece of the input signal must
 * be computed only once. This speeds up the convolution slightly. Further, assuming K kernels of
 * size M and a signal length of N samples. The direct FFT convolution cost would be
 * \f$(K+1)\,N\,\log(N)\f$, not including the costs of computing the FFTs of the kernels.
 * Using the overlap-add method, the costs are \f$(K+1)\,N\,\log(2\,M)\f$, which results into a
 * benifit if \f$2\,M < N\f$. */
class Convolution
{
public:
  /** Constructor.
   * @param kernels Specifies the kernels to use. */
  Convolution(const CMatrix &kernels);
  /** Destructor. */
  virtual ~Convolution();

  /** Performs the convolution of the signal passed by @c signal with the kernels passed to the
   * constructor. The results are stored in the columns of the array @c out. */
  template <class iDerived, class oDerived>
  void apply(const Eigen::DenseBase<iDerived> &signal, Eigen::DenseBase<oDerived> &out) {
    // First, clear _lastRes matrix
    this->_lastRes.setConstant(0);

    /*
     * Perform overlap-add FFT
     */
    size_t N = signal.size();
    size_t steps = N/this->_M + ( (N%this->_M) ? 1 : 0 );
    for (size_t i=0; i<steps; i++) {
      // Get the indices of the piece of the signal being processed [a,b), hence b-a is
      // the length of the piece
      size_t a = i*this->_M, b = std::min((i+1)*this->_M, N);
      size_t n = b-a, pad = 2*this->_M-b+a;
      // Store piece into forward trafo buffer
      this->_part.block(0,0, n,1).noalias() = signal.block(a,0, n,1);
      // 0-pad
      this->_part.tail(pad).setConstant(CScalar(0));
      // perform forward FFT
      this->_fwd.exec();

      // Multiply result of forward FFT of the signal piece with every (transformed) kernel
      for (size_t j=0; j<_K; j++) {
        this->_work.col(j).noalias() =
            this->_part.cwiseProduct(this->_kernelF.col(j).conjugate());
      }
      // Peform backward trafo
      this->_rev.exec();

      // Compute result of partial convolution and store it into the output buffer
      out.block(a,0, n,this->_K).noalias() =
          (this->_work.topLeftCorner(n,this->_K) +
           this->_lastRes.topLeftCorner(n,this->_K))/(2*this->_M);

      // Store remaining part for next step (if needed)
      if ((i+1)<steps) {
        this->_lastRes.noalias() = this->_work.block(this->_M,0, this->_M, this->_K);
      }
    }
  }

  /** Returns the length of the kernels. */
  inline size_t kernelLength() const { return this->_M; }
  inline size_t numKernels() const { return this->_K; }

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
  FFT _fwd;

  /** Second halfs of the back-transformed, filtered singals. */
  CMatrix _lastRes;
  /** Working memory for backward transformation of filtered signals. */
  CMatrix _work;
  /** Backward transformation. */
  FFT _rev;
};

}

#endif // __WT_CONVOLUTION_HH__
