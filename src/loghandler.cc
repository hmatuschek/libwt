#include "loghandler.hh"
#include <QTextStream>
#include <QFileInfo>

LogHandler::LogHandler()
  : QObject(), wt::LogHandlerObj(wt::LogMessage::LDEBUG)
{
  // pass...
}

void
LogHandler::handleMessage(const wt::LogMessage &msg) {
  QString buffer;
  QTextStream stream(&buffer);
  QFileInfo finfo(msg.filename().c_str());

  stream << finfo.fileName() << " line " << msg.linenumber() << ": " << msg.message().c_str();
  emit message(buffer);
}
