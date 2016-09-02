#include "application.hh"
#include <QFileDialog>
#include <QInputDialog>
#include "api.hh"
#include "utils/csv.hh"
#include "utils/logger.hh"
#include <fstream>
#include "transformdialog.hh"



Application::Application(int &argc, char **argv)
  : QApplication(argc, argv), _items(0)
{
  _items = new ItemModel(this);
}

Application::~Application() {
  // pass...
}

bool
Application::importTimeseries() {
  QString filename = QFileDialog::getOpenFileName(
        0, tr("Import timeseries"), "", tr("CSV (*.txt *.csv)"));
  if (! filename.size())
    return false;

  std::ifstream file;
  file.open(filename.toUtf8().constData());
  if(! file.is_open()) {
    logError() << "Cannot read data from '" << filename.toUtf8().constData()
               << "': Cannot open file for reading.";
    return false;
  }

  Eigen::MatrixXd data;
  wt::CSV::read(data, file);
  file.close();

  if ((0 == data.rows()) || (0 == data.cols())) {
    logError() << "Cannot read data from '" << filename.toUtf8().constData()
               << "': Malformed CSV file.";
    return false;
  }

  int column = 0;
  if (1 < data.cols()) {
    bool ok;
    int col = QInputDialog::getInt(0, tr("Select column"),
                                   tr("The file contains more than one column, "
                                      "select the column to import."),
                                   0, 0, data.cols(), 1, &ok);
    if (ok)
      column = col;
  }

  bool ok;
  double Fs = QInputDialog::getDouble(0, tr("Set sample rate"),
                                      tr("Specify the sample rate for the time series."),
                                      1, 0, 1e12, 3, &ok);
  if (! ok)
    Fs = 1;

  QFileInfo finfo(filename);
  QString name = finfo.baseName();
  _items->addItem(new TimeseriesItem(data.col(column), Fs, name));
  return true;
}

bool
Application::startTransform(TimeseriesItem *item) {
  TransformDialog dialog(item->data().size(), item->Fs());
  if (QDialog::Rejected == dialog.exec())
    return false;

  Eigen::VectorXd scales(dialog.numScales());
  double minScale = std::min(dialog.minScale(),dialog.maxScale());
  double maxScale = std::max(dialog.minScale(),dialog.maxScale());
  switch (dialog.scaling()) {
  case TransformDialog::LINEAR:
    wt::linear_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  case TransformDialog::DYADIC:
    wt::dyadic_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  case TransformDialog::DECADIC:
    wt::decadic_range(minScale*item->Fs(), maxScale*item->Fs(), scales);
    break;
  }
  wt::Wavelet wavelet;
  switch (dialog.wavelet()) {
  case TransformDialog::MORLET_WAVELET:
    wavelet = wt::Morlet(0.5);
    break;
  case TransformDialog::CAUCHY_WAVELET:
    wavelet = wt::Cauchy(20);
    break;
  }

  TransformItem *item = new TransformItem(item, wavelet, scales, this);
  _items->addItem(item);
  connect(item, SIGNAL(finished(TransformItem*)), this, SLOT(onTransformItemFinished(TransformItem*)));
  item->start();
  return true;
}


void
Application::onTransformItemFinished(TransformItem *item) {
  _items->remItem(item);
}

