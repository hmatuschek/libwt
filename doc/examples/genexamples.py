from numpy import *
import matplotlib.pyplot as plt

N = 2048
t = linspace(0.,1.,N)

X1 = sin(2*pi*20*t).reshape(N, 1)
savetxt("sin.csv", X1, delimiter="\t")

X2 = X1 + sin(2*pi*40*t).reshape(N, 1)
savetxt("sin2.csv", X2, delimiter="\t")

X3 = zeros((N,5))
X3[N//2,0] = 1
X3[:,1] = cumsum(X3[:,0])*blackman(N)
X3[:,2] = cumsum(cumsum(X3[:,0]))*blackman(N)
X3[:,3] = cumsum(cumsum(cumsum(X3[:,0])))*blackman(N)
X3[:,4] = cumsum(cumsum(cumsum(cumsum(X3[:,0]))))*blackman(N)
savetxt("sing.csv", X3, delimiter="\t")

savetxt("sin_noise.csv", X1+random.randn(N,1), delimiter="\t");

f = linspace(1,40,N)
X4 = sin(2*pi*f*t).reshape(N,1)
savetxt("sinsweep.csv", X4, delimiter="\t")

f = 100.+2*sin(2*pi*linspace(0.,1.,N))
X5 = cos(2*pi*f*t).reshape(N,1)
plt.plot(t,X5[:,])
plt.show()
savetxt("fm.csv", X5, delimiter="\t")

earth = loadtxt("earth.csv")
savetxt("earth.csv", earth, delimiter="\t")
savetxt("mmg.csv", loadtxt("mmg.csv"), delimiter="\t")
