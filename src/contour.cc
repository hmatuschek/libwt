#include "contour.hh"


Polygon
rk_contour(TransformedItem *item, double b, double a, double r) {
  double dt = 1./item->Fs();
  int i = std::max(0, std::min(item->data().rows()-1, int(b*item->Fs())));
  int j = 0;
  while ((j<item->scales().size()) && (a<=item->scales()(j)))
    j++;
  b = i*dt; a = item->scales()(j);

  // Walk left until end of plot or hit contour
  double rvalue = item->wavelet().evalRepKern(0,1);
  double value = rvalue;
  while ((r>value/rval) && (i<item->data().rows())) {
    i++; value = item->wavelet().evalRepKern((b-i*dt)/a,1);
  }
  Polygon result; result.add(i*dx, item->scales()(j));

}
