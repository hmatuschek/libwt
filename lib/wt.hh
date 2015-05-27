/** \mainpage C++ library for the continious wavelet transform.
 *
 * The continuous wavelet transfrom of a (possibly complex) signal \f$s(t)\f$, given a mother
 * wavelet \f$g(t)\f$, at time \f$b\f$ and  scale \f$a\f$ is given by
 *
 * \f[m(b,a) = \int_{-\infty}^{\infty}\bar{s}(t)\,\frac{1}{a}g\left(\frac{t-b}{a}\right)\,dt\,,\f]
 *
 * see @ref wavelets for a list of all implemented wavelets. For each proper
 * (analysis) wavelet\cite Holschneider1998 \f$g\f$, there exists a second (synthesis) wavelet
 * \f$h(t)\f$ that reconstructs the signal as
 *
 * \f[s(t) = \frac{1}{c_{g,h}}\,\int_{0}^{\infty}\,\frac{da}{a}\,\int_{-\infty}^{\infty}\,dt\,
 *  \bar{m}(b,a)\,\frac{1}{a}h\left(\frac{b-t}{a}\right)\,,\f]
 *
 * where \f$c_{g,h}\f$ is a normalization constant depending on the chosen wavelet pair \f$g,h\f$.
 *
 * \section implementation Efficient implementation of the continious wavelet transform.
 * In general, using the FFT convolution algorihtm to perform a wavelet transform of a signal of
 * length \f$N\f$ at \f$M\f$ scales, the computational costs (time) are generally
 * \f$O(M\,N\log N)\f$. Hence the tranform can get pretty slow for long signals. Exploiting, that
 * wavelets are usually well localized in the time and frequency domain, allows for a significant
 * reduction of the computational costs of the transform. This gain, however, is a constant
 * factor which means, that the general dependency of the costs on the signal length and number of
 * scales remains in the order of \f$O(M\,N\log N)\f$.
 *
 * At small scales, the time localization of the wavelet can be exploited. There the
 * scaled wavelet can be approximated well by a filter kernel beeing much shorter than the time
 * series. The convolution of the (short) scaled wavelet with the (long) time series can then be
 * performed efficiently by using the well known "overlap-add" method\cite Smith2012.
 *
 * At large scales, the frequency localization of the wavelet can be exploited. There
 * the high-frequency components of the wavelet are basically zero and the convolution of these
 * long but low-frequency wavelets can be performed in an interleaved manner on sub-samples of the
 * signal. This approach is related to the "algorithm a trois"\cite Holschneider1990 for the
 * wavelet transform on dyadic grids.
 *
 * In both cases, a time series of length \f$N\f$ will be processed in terms of \f$K\f$
 * signals of length \f$L\f$, such that \f$N = K\,L\f$ (assuming the signal length can be factored
 * that way, if not the signal will be zero-padded). Hence the computational costs of the FFT
 * convolutions to obtain a single "voice" (wavelet trasform at a specific frequency/scale)
 * are \f$O(K\,L\log L)=O(N\log L)\f$ instead of \f$O(N\log N)\f$. If \f$N\gg L\f$ the costs of
 * the wavelet transform can be reduced significantly.
 */

#ifndef __WT_HH__
#define __WT_HH__

#include "types.hh"
#include "wavelet.hh"
#include "wavelettransform.hh"
#include "waveletsynthesis.hh"

#endif // __WT_HH__
