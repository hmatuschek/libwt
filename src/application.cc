#include "application.hh"
#include <QFileDialog>


Application::Application(int &argc, char **argv)
  : QApplication(argc, argv), _items(0)
{
  _items = new ItemModel(this);
}

Application::~Application() {
  // pass...
}

void
Application::importTimeseries() {
  QString filename = QFileDialog::getOpenFileName(
        0, tr("Import timeseries"), "", tr("CSV (*.txt *.csv)"));
  if (! filename.size())
    return;
}

