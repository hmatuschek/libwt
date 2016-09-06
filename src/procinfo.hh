#ifndef PROCINFO_HH
#define PROCINFO_HH

#include <QProcess>
#include <time.h>
#include <sys/time.h>

class ProcInfo: public QObject
{
  Q_OBJECT

public:
  explicit ProcInfo(QObject *parent=0);

public slots:
  void update();

signals:
  void updated(double mem, double cpu, double time);

protected slots:
  void _onReadyRead();

protected:
  QProcess _process;
  clock_t _startTime;
};

#endif // PROCINFO_HH
