#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QStackedWidget>
#include <QItemSelection>
#include <QStatusBar>
#include "application.hh"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(Application &app, QWidget *parent = 0);

protected slots:
  void selectedItemChanged(size_t idx);
  void procStatUpdate(double mem, double cpu, double time);

protected:
  Application &_application;
  QStackedWidget *_viewstack;
  QStatusBar *_statusBar;
};

#endif // MAINWINDOW_HH
