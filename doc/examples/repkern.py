from numpy import *
import wt
from matplotlib import pylab

N       = 1024
Nscales = 64
scales = linspace(2, 100, Nscales)
t = linspace(-N/2,N/2, N)
W = wt.Cauchy(20)
X = array(map(W.evalAnalysis,t/20))

T = wt.WaveletTransform(W, scales)
wX = empty((N, Nscales), dtype=complex, order="F")
T(X, wX)

P = empty((N,Nscales), dtype="complex")
for i in range(N):
  for j in range(Nscales):
    P[i,j] = W.evalRepKern(t[i]/scales[j], 20/scales[j])/scales[j]

pylab.subplot(311)
pylab.contourf(t, scales, abs(wX).T, 30)
pylab.subplot(312)
pylab.contourf(t, scales, abs(P).T, 30)
pylab.subplot(313)
pylab.plot(linspace(0,1,N), abs(fft.fft(X)))
pylab.show()


