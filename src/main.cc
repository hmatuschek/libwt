#include "application.hh"
#include "mainwindow.hh"


int main(int argc, char *argv[]) {
  Application app(argc, argv);

  MainWindow *win = new MainWindow(app);
  win->show();

  app.exec();

  return 0;
}
