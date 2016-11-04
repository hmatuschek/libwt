from unittest import *
from numpy import *
from wt import *


class TestWaveletTransform(TestCase):
  """ Implements some simple tests for the wavelet transform. """
  def testTransform(self):
    # Delta peak
    N=128*1024;
    scale = 200.;
    Nscales = 10;
    signal = zeros((N,)); signal[N/2] = 1;
    # Perform WT of delta-peak on single scale
    #  -> evaluation of wavelet at that scale
    scales = empty((Nscales,)); scales[:] = scale;
    transformed = empty((N,Nscales), dtype=complex)

    wt = WaveletTransform(Morlet(), scales);
    wt(signal, transformed);
    for i in range(N):
      self.assertAlmostEqual(transformed[i,0].real(),
        wavelet().evalAnalysis(float(i-N/2)/scale).real()/scale, 5)
      self.assertAlmostEqual(transformed[i,0].imag(),
        wavelet().evalAnalysis(float(i-N/2)/scale).imag()/scale, 5)


if __name__ == "__main__":
  main();
