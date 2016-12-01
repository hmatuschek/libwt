"""
This module provides an implementation the Wilson algorithm for factorization of spectral matrices
as described in [Wilson72]_.

.. moduleauthor:: Hannes Matuschek

.. [Wilson72] Wilson, G. T.: The Factorization of Matricial Spectral Densities.
   SIAM Journal on Applied Mathematics, Vol. 23, No. 4, pp. 420-426, 1972.
"""
import numpy
from numpy import dot, empty, real, mean, diag, empty_like, conj
from numpy.linalg import cholesky, inv


def _positive_frequencies(f):
    """.. todo:: Describe."""
    # Calc FFT coefficients 
    N = len(f); F = numpy.fft.fft(f)
    # Set all negative frequencies to 0.
    for i in xrange(N/2+1, N):
        F[i] = 0.0
    F[0] /= 2.0
    return numpy.fft.ifft(F)


    
def wilson_fact(S, m=5):
    """ Implements the Wilson algorithm for spectral factorization as described in [Wilson72]_.

    The article describes two variants (analytic and algebraic), this function implements the
    'analytic' variant.

    :param S: Specifies the spectral matrizes (q x q x n).
    :param m: Specifies the number of iterations.
    """
    # Make initial guess for psi_0(z) = H (const), where H*H.T = gamma_0:
    # where gamma_0 = mean(S, axis=2)
    psi = empty_like(S); tmp = 2*real(cholesky(mean(S, axis=2)).T)
    for i in xrange(psi.shape[2]):
        psi[:,:,i] = tmp 

    # Approximate decomposition recursively:
    for k in xrange(m):
        tmp = empty_like(S)

        for i in xrange(tmp.shape[2]):
            inv_psi = inv(psi[:,:,i])
            tmp[:,:,i] = dot(dot(inv_psi, S[:,:,i]), conj(inv_psi.T)) + diag((1.,)*psi.shape[0])

        for i in  xrange(tmp.shape[0]):
            for j in xrange(tmp.shape[1]):
                tmp[i,j,:] = _positive_frequencies(tmp[i,j,:])
                
        C = -mean(tmp, axis=2); C[0,1] = -conj(C[1,0])
        C[0,0] = 0.; C[1,1] = 0.
        C = C/tmp.shape[2]

        for i in xrange(tmp.shape[2]):
            tmp[:,:,i] += C

        for i in xrange(tmp.shape[2]):
            psi[:,:,i] = dot(psi[:,:,i], tmp[:,:,i])

    return psi


#
# List exported functions
#
__all__ = ["wilson_fact"]
