#ifndef LOGHANDLER_HH
#define LOGHANDLER_HH

#include <QObject>
#include "utils/logger.hh"


class LogHandler: public QObject, public wt::LogHandlerObj
{
  Q_OBJECT

public:
  LogHandler();

  void handleMessage(const wt::LogMessage &msg);

signals:
  void message(const QString &msg);
};

#endif // LOGHANDLER_HH
