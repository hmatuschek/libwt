import unittest
from numpy import *
import wt


class TestWaveletTransform(unittest.TestCase):
  """ Implements some simple tests for the wavelet transform. """

  def test_transform(self):
    # Delta peak
    N=128*1024;
    scale = 200.;
    Nscales = 1;
    signal = zeros((N,)); signal[N/2] = 1;
    # Perform WT of delta-peak on single scale
    #  -> evaluation of wavelet at that scale
    scales = empty((Nscales,)); scales[:] = scale;
    transformed = empty((N,Nscales), dtype=complex)
    wavelet = array(map(wt.Morlet().evalAnalysis, (arange(N)-N/2)/scale))/scale
    WT = wt.WaveletTransform(wt.Morlet(), scales);
    WT(signal, transformed);

    self.assertTrue(allclose(transformed[:,0], wavelet, atol=1e-5))


class TestWaveletSynthesis(unittest.TestCase):
  """ Implements some simple tests for the wavelet synthesis. """

  def test_synthesis(self):
    # Delta peak
    N=1024;
    Nscales = 64;
    signal = zeros((N,)); signal[N/2] = 1;
    # Perform WT of delta-peak on single scale
    #  -> evaluation of wavelet at that scale
    scales = linspace(2,100, Nscales)
    transformed = empty((N, Nscales), dtype=complex);
    WT = wt.WaveletTransform(wt.Cauchy(16), scales);
    WT(signal, transformed);

    reconst = empty((N,), dtype=complex);
    WS = wt.WaveletSynthesis(WT);
    WS(transformed, reconst);
    # pretty bad anyway
    self.assertTrue(allclose(reconst.real, signal, rtol=1e-2, atol=1e-1))


if __name__ == "__main__":
  suite = unittest.TestSuite()
  suite.addTest(unittest.defaultTestLoader.loadTestsFromTestCase(TestWaveletTransform))
  suite.addTest(unittest.defaultTestLoader.loadTestsFromTestCase(TestWaveletSynthesis))
  unittest.TextTestRunner(verbosity=2).run(suite)
