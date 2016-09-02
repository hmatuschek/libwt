#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QStackedWidget>
#include <QItemSelection>
#include "application.hh"


class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(Application &app, QWidget *parent = 0);

protected slots:
  void selectedItemChanged(const QItemSelection &current, const QItemSelection &previous);

protected:
  Application &_application;
  QStackedWidget *_viewstack;
};

#endif // MAINWINDOW_HH
