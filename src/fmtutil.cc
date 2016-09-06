#include "fmtutil.hh"
#include <QObject>
#include <cmath>


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
    return QObject::tr("%0b").arg(int(v));
  if (v < 1048576.)
    return QObject::tr("%0kb").arg(QString::number(v/1024, 'f', 1));
  if (v < 1073741824.)
    return QObject::tr("%0Mb").arg(QString::number(v/1048576., 'f', 1));
  return QObject::tr("%0Gb").arg(QString::number(v/1073741824., 'f', 1));
}

QString
fmt_time(double t) {
  if (std::abs(t) < 1e-3)
    return QObject::tr("%0us").arg(QString::number(t*1e6));
  if (std::abs(t) < 1.)
    return QObject::tr("%0ms").arg(QString::number(t*1000));
  if (std::abs(t)<90)
    return QObject::tr("%0s").arg(QString::number(t, 'f', 2));
  if (std::abs(t)<5400)
    return QObject::tr("%0min").arg(QString::number(t/60, 'f', 2));
  return QObject::tr("%0h").arg(QString::number(t/3600, 'f', 2));
}
