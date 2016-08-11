from numpy import *
import wt
from matplotlib import pylab

N       = 1024
Nscales = 64
scales = linspace(2, 100, Nscales)
t = linspace(-N/2,N/2, N)
X = sin(2*pi*t/20)/(2*pi*t/20)

T = wt.WaveletTransform(wt.Cauchy(20), scales)
wX = empty((N, Nscales), dtype=complex, order="F")
T(X, wX)

S = wt.WaveletSynthesis(T)
rwX = empty((N,), dtype=complex)
S(wX, rwX)

P = wt.WaveletConvolution(T)
pwX = empty((N, Nscales), dtype=complex, order="F")
P(wX, pwX)

wrwX = empty((N,Nscales), dtype=complex, order="F")
T(rwX, wrwX)

pylab.subplot(521)
pylab.plot(t, X)
pylab.subplot(523)
pylab.contourf(t, scales, abs(wX).T, 30)
pylab.subplot(524)
pylab.contourf(t, scales, angle(wX).T, 30)
pylab.subplot(525)
pylab.plot(t, rwX.real)
pylab.subplot(526)
pylab.plot(t, rwX.imag)
pylab.subplot(527)
pylab.contourf(t, scales, abs(pwX).T, 30)
pylab.subplot(528)
pylab.contourf(t, scales, angle(pwX).T, 30)
pylab.subplot(529)
pylab.contourf(t, scales, abs(wrwX).T, 30)
pylab.subplot(5,2,10)
pylab.contourf(t, scales, angle(wrwX).T, 30)
pylab.show()


