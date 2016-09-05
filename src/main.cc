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
    app.importTimeseries(argv[1]);
  }

  app.exec();

  return 0;
}
