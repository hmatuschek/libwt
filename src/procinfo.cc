#include "procinfo.hh"
#include "utils/logger.hh"
#include <QStringList>
#include <QtGlobal>
#include <QApplication>


ProcInfo::ProcInfo(QObject *parent)
  : QObject(parent), _process(), _startTime(clock())
{
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
  _process.setProgram("/bin/ps");
  QStringList args;
  args << "-p" << QString::number((qlonglong)QApplication::applicationPid())
       << "-o" << "rss,%cpu";
  _process.setArguments(args);
#else
  logError() << "Cannot obtain process info, platform not supported.";
#endif

  connect(&_process, SIGNAL(readyReadStandardOutput()), SLOT(_onReadyRead()));
}

void
ProcInfo::update() {
  if (QProcess::NotRunning == _process.state())
    _process.start(QIODevice::ReadOnly);
}

void
ProcInfo::_onReadyRead() {
  double mem = qQNaN(), cpu = qQNaN();
#if defined(Q_OS_MAC) || defined(Q_OS_LINUX)
  QString line = _process.readLine();
  line = _process.readLine();
  QStringList values = line.split(" ", QString::SkipEmptyParts);
  mem = values.at(0).toInt()*1024.;
  cpu = values.at(1).toDouble()/100.;
#endif
  clock_t now = clock();
  double dt = now-_startTime;
  emit updated(mem, cpu, dt/CLOCKS_PER_SEC);
}
