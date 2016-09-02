#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QStackedWidget>

#include "application.hh"


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(Application &app, QWidget *parent = 0);

protected:
  Application &_application;
  QStackedWidget *_viewstack;
};

#endif // MAINWINDOW_HH
