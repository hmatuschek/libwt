#include "fmtutil.hh"

QString
fmt_freq(double f) {
  if (f < 10)
    return QString("%0Hz").arg(QString::number(f, 'f', 3));
  if (f<1e3)
    return QString("%0Hz").arg(QString::number(f, 'f', 1));
  if (f<1e6)
    return QString("%0kHz").arg(QString::number(f/1e3,'f',1));
  if (f<1e9)
    return QString("%0MHz").arg(QString::number(f/1e6,'f',1));
  return QString("%0GHz").arg(QString::number(f/1e9,'f',1));
}


QString
fmt_cpu(double v) {
  return QString("%0%").arg(int(v*100));
}

QString
fmt_mem(double v) {
  if (v < 1024)
    return QString("%0b").arg(int(v));
  if (v < 1048576.)
    return QString("%0kb").arg(QString::number(v/1024, 'f', 1));
  if (v < 1073741824.)
    return QString("%0Mb").arg(QString::number(v/1048576., 'f', 1));
  return QString("%0Gb").arg(QString::number(v/1073741824., 'f', 1));
}
