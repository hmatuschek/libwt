#ifndef PROCINFO_HH
#define PROCINFO_HH

#include <QProcess>

class ProcInfo: public QObject
{
  Q_OBJECT

public:
  explicit ProcInfo(QObject *parent=0);

public slots:
  void update();

signals:
  void updated(double mem, double cpu);

protected slots:
  void _onReadyRead();

protected:
  QProcess _process;
};

#endif // PROCINFO_HH
