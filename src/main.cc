#include "application.hh"
#include "mainwindow.hh"
#include "utils/logger.hh"

int main(int argc, char *argv[])
{
  wt::Logger::addHandler(wt::IOLogHandler(std::cerr, wt::LogMessage::LDEBUG));

  Application app(argc, argv);


  MainWindow *win = new MainWindow(app);
  win->show();

  if (argc>1) {
    QString arg = argv[1];
    if (arg.endsWith(".h5"))
      app.addSession(arg);
    else
      app.importTimeseries(arg);
  }

  app.exec();

  return 0;
}
